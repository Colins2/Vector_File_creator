//---------------------------------------------------------------------------
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
//#include <math.h>
#include <vector>
//#include <memory.h>
#pragma hdrstop
#include "shape.h"
#include "Vec_creator.h"
#include "Shapewriter.h"
#include "VFCheaders.h"
#include "vecstore.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)


//---------------------------------------------------------------------------
Shpwriter::~Shpwriter()
{
   //needed?
	shmsg = L"shpwriter object deleted";
	Form1->log.writelog(shmsg.c_str());
}
//---------------------------------------------------------------------------
Shpwriter::Shpwriter()
{
	//needed?
	shmsg = L"new shpwriter object created";
	Form1->log.writelog(shmsg.c_str());
}
//---------------------------------------------------------------------------
void Shpwriter::ShpMain()
{
	//main function
	//Shpwriter *shw = new Shpwriter;
	//do some stuff

	//delete shw;
	//return
}
//--------------------------------------------------------------------------
void Shpwriter::vstrnset(char *buffer, char ch, int count)
{
	int bufflen = strlen(buffer);
	int i = 0;
	if(count > bufflen)
		count = bufflen;
	do{
		buffer[i] = ch;
		i++;
	}while(i < count);

	return;
}
//--------------------------------------------------------------------------
void Shpwriter::Writeshapefile(vector<LatLonPoint> llp, int numpts)
{    //maybe bool()?
	//we get here called from another file after some basic setup has been
	//done for the shapefile set. Some further setup needs performing such
	//as filename creation etc.
	String Shp = filestem + L".shp";
	String Shx = filestem + L".shx";
	String Dbf = filestem + L".dbf";
	String Prj = filestem + L".prj";
	/*
	char prjtext [] = {"GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",
					SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],
					PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]"};
	 */
	//filenames OK
	struct tm *sysdate;
	time_t loctm;
	//make sure we can open the files first
	ofstream oshp(Shp.c_str(), ios::binary);
	if(!oshp){
		Application->MessageBox(L"Failed to create Shapefile",
								Form1->VCmsg.c_str(), MB_OK);
		return;
		}
	ofstream oshx(Shx.c_str(), ios::binary);
	if(!oshx){
			Application->MessageBox(L"Failed to create index file",
				Form1->VCmsg.c_str(), MB_OK);
			if(oshp)
			oshp.close();
		return;
		}
	ofstream odbf(Dbf.c_str(), ios::binary);
	if(!odbf){
			Application->MessageBox(L"Failed to create database file",
					Form1->VCmsg.c_str(), MB_OK);
			if(oshp)
				oshp.close();
			if(oshx)
				oshx.close();
			return;
		}

	 ofstream oprj(Prj.c_str(), ios::binary);
		if(!oprj){
			Application->MessageBox(L"Failed to create projection file",
					Form1->VCmsg.c_str(), MB_OK);
			if(oshp)
				oshp.close();
			if(oshx)
				oshx.close();
			if(odbf)
				odbf.close();
			return;
		}
	// write the prj file first as that is straightforward
	oprj << "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",";
	oprj << "SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM";
	oprj << "[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]";

	oprj.close();

	//Now to make the shapefile set filenames exist
	dbf_header *dbfh = new dbf_header;
	dbf_field_desc *dbffd = new dbf_field_desc;
	dbfxprec *dbfr = new dbfxprec;
	shp_header *shph = new shp_header;
	shp_rec_header *shprh = new shp_rec_header;
	shp_poly_rec *spr = new shp_poly_rec;
	polyb *spp = new polyb;
	shx_rec *shxr = new shx_rec;

	double xmax = 0, xmin = 0, ymax = 0, ymin = 0;
	loctm = time(NULL);
	sysdate=localtime(&loctm);
	//fill the dbf header
	dbfh->dbhstart= 0x03;
	dbfh->day=sysdate->tm_mday;
	dbfh->month=sysdate->tm_mon+1;
	dbfh->year=sysdate->tm_year;
	dbfh->reccount=1; //only 1 record
	dbfh->head_bytes=97;  //depends on how many field descriptors there are.
					   //32 byte header + x no. of 32 byte descriptors + 0x0d
					   //only 2 field descriptors this time
	dbfh->data_width = sizeof(dbfxprec);
	//strset(dbfh->fill, NULL);
	memset(dbfh->fill, NULL, sizeof(dbfh->fill));
	odbf.write((char *)dbfh, sizeof(dbf_header));
	//field descriptor
	memset(dbffd->field_name, 0x0, sizeof(dbffd->field_name));
	strncpy(dbffd->field_name, "Name", 4); //don't want trailing \0
	dbffd->field_type = 'C';
	dbffd->fill = 0;
	dbffd->field_len = 20;
	dbffd->field_decpl = 0;
	dbffd->res = 0;
	dbffd->waid = NULL;
	dbffd->res2 = 0;
	dbffd->set_fields_flag = NULL;
	//strset(dbffd->reserv, NULL);
	memset(dbffd->reserv, NULL, sizeof(dbffd->reserv));
	odbf.write((char *)dbffd, sizeof(dbf_field_desc));
	//make 2nd fd
	memset(dbffd->field_name, 0x0, sizeof(dbffd->field_name));
	strncpy(dbffd->field_name, "Feature", 7);
	dbffd->field_len = 12;
	odbf.write((char *)dbffd, sizeof(dbf_field_desc));

	odbf.put(0x0d);
	shmsg = L"DBF file created";
	Form1->log.writelog(shmsg.c_str());

	shmsg = L"Creating Shapefile and Index";
	Form1->log.writelog(shmsg.c_str());

	//shph->fil_code = SWAP_INT32(0x270A);
	memset(shph->fil_code, 0x0, 4);
	shph->fil_code[2] = 0x27;
	shph->fil_code[3] = 0x0a;
	shph->unu1 = shph->unu2 = shph->unu3 = shph->unu4 = shph->unu5 = 0;
	shph->fil_len = SWAP_INT32(20);   //have to be calculated and re-written later
	shph->fil_ver = 1000;
	shph->shp_typ = 5;
	//x axis is longitude, y axis is latitude.
	shph->xmin = wbound;
	shph->ymin = sbound;
	shph->xmax = ebound;
	shph->ymax = nbound;
	shph->zmin = shph->zmax = shph->mmin = shph->mmax = 0;
	//write SHP header
	oshp.write((char *)shph, sizeof(shp_header));
	//write SHX header
	oshx.write((char *)shph, sizeof(shp_header));
	shmsg = L"Shapefile and index file created";
	Form1->log.writelog(shmsg.c_str());
	//OK to here
	//shp
	shprh->recno = SWAP_INT32(1); //only 1 record
	//content = header size =
	//record header = 44 bytes + 4 * numparts 1 loop so just 4 bytes +
	//number of points * 2 * 8 bytes
	// add them all and divide by 2 to get words
	//shprh->content = SWAP_INT32((44 + 4 + 16 + (tvph->numpts * 2 * 8))/2);
								//=(44 + 4 + 16 + (5 * 2 * 8))/2
	shprh->content = SWAP_INT32((44 + 4 * 1 +(numpts * 2 * 8))/2);
	//OK for single polygon but if more then need to get numpts/numparts first
//shx
	shxr->shp_offset = SWAP_INT32((oshp.tellp()/2)); //offset in words
	shxr->content = shprh->content; //already swapped

//dbf
	dbfr->del_mark = 0x20;
	UnicodeToUtf8(dbfr->name, Shpname.c_str(), 20);
	UnicodeToUtf8(dbfr->feature, Shpfeature.c_str(),12);
//write out the record headers
	oshp.write((char *)shprh, sizeof(shp_rec_header));
	oshx.write((char *)shxr, sizeof(shx_rec));
	odbf.write((char *)dbfr, sizeof(dbfxprec));

 //get the poly record ready
	 spr->shp_typ = 5;
	 spr->bxmin = wbound;
	 spr->bymin = sbound;
	 spr->bxmax = ebound;
	 spr->bymax = nbound;
	 spr->numparts = 1;
	 spr->numpoints = numpts;
	 spr->parts = 0;
//write the poly record header
	oshp.write((char *)spr, sizeof(shp_poly_rec));
//create an iterator
	vector<LatLonPoint>::iterator llpi = llp.begin();
// now write out the points from the vector
   llpi = llp.begin();
   while(llpi != llp.end()){
		spp->xpt = llpi->lon;
		spp->ypt = llpi->lat;
		oshp.write((char *)spp, sizeof(polyb));
		llpi++;
   }

   //get the file lengths and re-write the headers
	shph->fil_len = SWAP_INT32(oshp.tellp() / 2);
	oshp.seekp(0, ios::beg);
	oshp.write((char *)shph, sizeof(shp_header));
	oshx.seekp(0, ios::beg);
	shph->fil_len = SWAP_INT32(50 + (1 * 4));
	oshx.write((char *)shph, sizeof(shp_header));
//should be it!
	shmsg = L"Completed";
	Form1->log.writelog(shmsg.c_str());

	delete dbffd;
	delete dbfh;
	delete dbfr;
	delete shph;
	delete shprh;
	delete spr;
	delete spp;
	delete shxr;
	//delete llpp;

	if(oshp)
		oshp.close();
	if(oshx)
		oshx.close();
	if(oshx)
		odbf.close();
	//copy the default prj file with the shapefile set name.

	return;




}
//-------------------------------------------------------------------------
