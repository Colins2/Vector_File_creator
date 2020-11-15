//---------------------------------------------------------------------------
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <dir.h>
#include <sys/stat.h>
#include <time.h>
#include <vector>

#pragma hdrstop
#include "Vec_creator.h"
#include "VFCheaders.h"
#include "Gridwriter.h"
#include "Apt_area.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

//---------------------------------------------------------------------------
/*
We will just ask for the *.vec file and work from that
*/

//---------------------------------------------------------------------------
Gridwriter::Gridwriter()
{
	gmsg = _D("gridwriter object created");
	Form1->log.writelog(gmsg.c_str());
}
//--------------------------------------------------------------------------
Gridwriter::~Gridwriter()
{
	gmsg = _D("gridwriter object destroyed");
	Form1->log.writelog(gmsg.c_str());
}
//-------------------------------------------------------------------------
void Gridwriter::ExtractAptName(char *s)
{
	char temp[MAXFILE];
	int bp = 0;//miss off the "V_", just want the airport name
	char ch;
	do{
		ch = s[bp+2];
		if(ch == '_'){
			temp[bp] = 0x0;
			break;
			}
		temp[bp] = ch;
		bp++;
	}while(ch != '_');
	strcpy(s, temp);
	return;
}
//--------------------------------------------------------------------------
void Gridwriter::Writegrid()
{
	//main routine
	struct tm *sysdate;
	time_t loctm;

	char cvecfn[MAXFILE];
	char dbfaname[20] = {};
	wchar_t wVecname[MAXFILE];
	wchar_t wvdr[MAXDRIVE];
	wchar_t wvpath[MAXPATH];
	wchar_t wvecfn[MAXFILE];
	wchar_t wvext[MAXEXT];
	wchar_t gridfn[20];
	wchar_t gridbase[MAXFILE];
	//String shpfile;
	//String shxfile;
	//String dbffile;
	wchar_t wshpfile[MAXFILE];
	wchar_t wshxfile[MAXFILE];
	wchar_t wdbffile[MAXFILE];
	wchar_t wprjfile[MAXFILE];
	wchar_t wgrdfile[MAXFILE];
	//get the vec file name to open
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Title = _D("Select the Airport vector file");
	//Form1->OpenDialog1->Filter = _D("KML files (*.kml) | *.KML");
	Form1->OpenDialog1->Filter = _D("vector files (*.vec) | *.VEC");
	if(Form1->OpenDialog1->Execute()){
		wcscpy(wVecname, Form1->OpenDialog1->FileName.c_str());
	}else{
		return;
	}
	//make sure we can open the vec file
	ifstream invec(wVecname, ios::binary);
		if(!invec){
			Application->MessageBox(_D("Could not open vector file"),
				Form1->VCmsg.c_str(), MB_OK);
				return;
		}
	//shapefile set filenames
	//UnicodeToUtf8(cvecfn, Vecname.c_str(), MAXFILE);
	//maybe try with wchar_t instead?
	_wfnsplit(wVecname, wvdr, wvpath, wvecfn, wvext);
	//filename will be "V_name_Airport_Area"
	UnicodeToUtf8(cvecfn, wvecfn, MAXFILE);
	ExtractAptName(cvecfn);
	strcpy(dbfaname, cvecfn);
	strcat(cvecfn, "_Grid");
	Utf8ToUnicode(wvecfn, cvecfn, MAXFILE);
	//wvecfn now "name_Grid"
	//make shapefile set
	_wfnmerge(wshpfile, wvdr, wvpath, wvecfn, _D(".shp") );
	_wfnmerge(wshxfile, wvdr, wvpath, wvecfn, _D(".shx") );
	_wfnmerge(wdbffile, wvdr, wvpath, wvecfn, _D(".dbf") );
	_wfnmerge(wprjfile, wvdr, wvpath, wvecfn, _D(".prj") );
	_wfnmerge(wgrdfile, wvdr, wvpath, wvecfn, _D(".grd") );

	//make the vector file data structs
	VFC_vector_file_header *vfh = new VFC_vector_file_header;
	VFC_polygon_header *vph = new VFC_polygon_header;
	//VFC_poly_point *vpp = new VFC_poly_point; //not needed
	//read the data in and take the bits we want
	//there is only 1 record
	invec.read((char *)vfh, sizeof(VFC_vector_file_header));
	invec.read((char *)vph, sizeof(VFC_polygon_header));
	//only want the bounding box so we can close now.
	invec.close();
	double nbound = vph->nbound;
	double sbound = vph->sbound;
	double wbound = vph->wbound;
	double ebound = vph->ebound;
	double gnlat, gslat, gwlon, gelon  = 0; // for line endpoints
	double pixsize = 1.0/1200;  //for both lat and lon
	int tiln, tils, tile, tilw = 0; //eddges of the tile.
	double demn, dems, deme, demw = 0; //edges of the dem ???
	int awidth, aheight = 0;
	//dem points are located on the edges of the tile so the pixels
	//straddle the tile boundaries.
	tils = (int)sbound;
		if(tils < 0)
			tils -=1;
	tiln = tils + 1;
	tilw = (int)wbound;
		if(tilw < 0)
			tilw -=1;
	tile = wbound + 1;
	demn = (double)tiln + (pixsize / 2);
	dems = (double)tils - (pixsize / 2);
	demw = (double)tilw - (pixsize / 2);
	deme = (double)tile + (pixsize / 2);
	//calculate the start/end points of our grid from SW corner to NE?
	int westpx = 0, eastpx = 0, northpx = 0, southpx = 0; //number of pixels to miss
	int xrange = 0, yrange = 0;
	westpx = (wbound - demw)/pixsize;
	gwlon = demw + (pixsize * westpx);//should be the longitude west of the bounds

	eastpx = ((ebound - demw)/pixsize) + 2;
	gelon = demw + (pixsize * (eastpx-1));
	xrange = abs(eastpx - westpx);   //for the writing loop
	//xrange = diff(eastpx, westpx);

	southpx = (sbound - dems)/pixsize;
	gslat = dems + (pixsize * southpx);

	northpx = ((nbound - dems)/pixsize) + 2;
	gnlat = dems + (pixsize * (northpx-1));
	yrange = abs(northpx - southpx); //for the writing loop
	//yrange = diff(northpx, southpx);
	//open up the grid data file and write the data
	ofstream ogrd(wgrdfile, ios::binary);
		if(!ogrd){
			//delete the structs
			Application->MessageBox(_D("Could not create gridfile"),
				Form1->VCmsg.c_str(), MB_OK);
				delete vfh;
				delete vph;
				return;
		}
	Grd_header *gh = new Grd_header;
	Grd_data *gd = new Grd_data;
	memset(gh->grd_magic, NULL, sizeof(gh->grd_magic));
	strcpy(gh->grd_magic, "XPUtils-Grid");
	gh->recnum = (yrange - 1) * (xrange - 1);
	ogrd.write((char *)gh, sizeof(Grd_header));
	gd->eastpix = eastpx;
	gd->westpix = westpx;
	gd->northpix = northpx;
	gd->southpix = southpx;
	gd->xrange = xrange - 1; //xrange is gridlines - we want spaces between
	gd->yrange = yrange - 1; //                 "

	ogrd.write((char *)gd, sizeof(Grd_data));

	//open up the shapefile set and write headers etc.
	ofstream oshp(wshpfile, ios::binary);
		if(!oshp){
			//delete the structs
			Application->MessageBox(_D("Could not create shapefile"),
				Form1->VCmsg.c_str(), MB_OK);
			delete vfh;
			delete vph;
			//delete vpp;
			return;
		}

	ofstream oshx(wshxfile, ios::binary);
		 if(!oshx){
			Application->MessageBox(_D("Could not create shapefile index"),
				Form1->VCmsg.c_str(), MB_OK);
			delete vfh;
			delete vph;
			//delete vpp;
			oshp.close();
			return;
		}
   ofstream odbf(wdbffile, ios::binary);
		if(!odbf){
			Application->MessageBox(_D("Could not create database file"),
				Form1->VCmsg.c_str(), MB_OK);
			delete vfh;
			delete vph;
			//delete vpp;
			oshp.close();
			oshx.close();
			return;
		}
   ofstream oprj(wprjfile, ios::binary);
		if(!oprj){
			Application->MessageBox(_D("Could not create projection file"),
				Form1->VCmsg.c_str(), MB_OK);
			delete vfh;
			delete vph;
			//delete vpp;
			oshp.close();
			oshx.close();
			odbf.close();
			return;
		}
//all output files open

//write the prj file
		oprj << "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",";
		oprj << "SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM";
		oprj << "[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]";

		oprj.close();
	 int recnum = 1; //numbering starts at 1
	 int numpts = 2; //fixed for this file
//now make some structs
	dbf_header *dbfh = new dbf_header;
	dbf_field_desc *dbffd = new dbf_field_desc;
	dbfxprec *dbfr = new dbfxprec;
	shp_header *shph = new shp_header;
	shp_rec_header *shprh = new shp_rec_header;
	shp_poly_rec *spr = new shp_poly_rec; //can use for polyline also
	polyb *spp = new polyb;
	shx_rec *shxr = new shx_rec;
//time to write stuff

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
	dbffd->field_len = 20;  //Airport name
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
	gmsg = L"DBF file created";
	Form1->log.writelog(gmsg.c_str());

	gmsg = L"Creating Shapefile and Index";
	Form1->log.writelog(gmsg.c_str());

	memset(shph->fil_code, 0x0, 4);
	shph->fil_code[2] = 0x27;
	shph->fil_code[3] = 0x0a;
	shph->unu1 = shph->unu2 = shph->unu3 = shph->unu4 = shph->unu5 = 0;
	shph->fil_len = SWAP_INT32(20);   //have to be calculated and re-written later
	shph->fil_ver = 1000;
	shph->shp_typ = 3; //polyline
	//x axis is longitude, y axis is latitude.
	shph->xmin = wbound;//demw;
	shph->ymin = sbound;//dems;
	shph->xmax = ebound;//deme;
	shph->ymax = nbound;//demn;
	shph->zmin = shph->zmax = shph->mmin = shph->mmax = 0;
	//write SHP header
	oshp.write((char *)shph, sizeof(shp_header));
	//write SHX header
	oshx.write((char *)shph, sizeof(shp_header));
	gmsg = L"Shapefile and index file created";
	Form1->log.writelog(gmsg.c_str());
	//all the headers written

 //some fixed values:
	shprh->content = SWAP_INT32((44 + 4 * 1 +(numpts * 2 * 8))/2);
	shxr->content = shprh->content; //already swapped
	dbfr->del_mark = 0x20;
	memset(dbfr->name, NULL, sizeof(dbfr->name));
	strcpy(dbfr->name, dbfaname);
	memset(dbfr->feature, NULL, sizeof(dbfr->feature));
	strcpy(dbfr->feature, "Gridline");
	spr->shp_typ = 3;
	spr->bxmin = wbound;       //try leaving the boundaries here
	spr->bymin = sbound;       //              "
	spr->bxmax = ebound;       //              "
	spr->bymax = nbound;       //              "
	spr->numparts = 1;
	spr->numpoints = numpts;   //always going to be 2
	spr->parts = 0;

	//loop
	//put the latitudes in an array?
	double grlats[yrange];
	for(int i = 0; i < yrange; i++){
		grlats[i] = gslat + (pixsize * i);
	}
	double grlons[xrange];
	for(int i = 0; i < xrange; i++){
		grlons[i] = gwlon + (pixsize * i);
	}

	int pxrange = xrange - 1;
	int pyrange = yrange - 1;

	double pixlats[pyrange];
	double pixlons[pxrange];
	//starting from sw corner
	for(int i= 0; i < pyrange; i++){
		pixlats[i] = grlats[i] + (pixsize / 2);
	}
	for(int i = 0; i < pxrange; i++){
		pixlons[i] = grlons[i] + (pixsize / 2);
	}
	//now to make pairs of coordinates
	LatLonPoint *llp = new LatLonPoint;
	//vector<LatLonPoint> vllp;
	//vector<LatLonPoint>::iterator iv;
	for(int i = pyrange-1; i >= 0; i--){
		for(int j = 0; j < pxrange; j++){
			llp->lat = pixlats[i];
			llp->lon = pixlons[j];
			//vllp.push_back(llp);
			ogrd.write((char *)llp, sizeof(LatLonPoint));
		}
	}

	//hopefully all in the array now in the correct order
	//close the grid file
	ogrd.close();
	Form1->aptarea.GridFile = wgrdfile;
	//write the grid details here?
	//best not. if no filename, say after restart, get it from OpenDialog

	delete gh;
	delete gd;
	delete llp;
	gmsg = _D("Grid header file written");
	Form1->log.writelog(gmsg.c_str());


	//draw e/w gridlines using lat array
	for(int i = 0; i < yrange; i++){

	//shp
	shprh->recno = SWAP_INT32(recnum); //only 1 record
	//content = header size =
	//record header = 44 bytes + 4 * numparts 1 loop so just 4 bytes +
	//number of points * 2 * 8 bytes
	// add them all and divide by 2 to get words
	//shprh->content = SWAP_INT32((44 + 4 + 16 + (tvph->numpts * 2 * 8))/2);
								//=(44 + 4 + 16 + (5 * 2 * 8))/2
	//shprh->content = SWAP_INT32((44 + 4 * 1 +(numpts * 2 * 8))/2); --always the same
	//OK for single polygon but if more then need to get numpts/numparts first
//shx
	shxr->shp_offset = SWAP_INT32((oshp.tellp()/2)); //offset in words
	//shxr->content = shprh->content; //already swapped --always the same

//dbf
	//dbfr->del_mark = 0x20;
	//UnicodeToUtf8(dbfr->name, Shpname.c_str(), 20);
	//UnicodeToUtf8(dbfr->feature, Shpfeature.c_str(),12);
	//strcpy(dbfr->name, "Gridline"); same every time
//write out the record headers
	oshp.write((char *)shprh, sizeof(shp_rec_header));
	oshx.write((char *)shxr, sizeof(shx_rec));
	odbf.write((char *)dbfr, sizeof(dbfxprec));
  //get the poly record ready
  /* done before the loop
	 spr->shp_typ = 3;
	 spr->bxmin = wbound;       //try leaving the boundaries here
	 spr->bymin = sbound;       //              "
	 spr->bxmax = ebound;       //              "
	 spr->bymax = nbound;       //              "
	 spr->numparts = 1;
	 spr->numpoints = numpts;   //always going to be 2
	 spr->parts = 0;
	 */
//write the poly record header
	oshp.write((char *)spr, sizeof(shp_poly_rec));
	//now populate the points - 2 off each line
	//west to east
	spp->xpt = gwlon;
	spp->ypt = grlats[i];
	oshp.write((char *)spp, sizeof(polyb));
	//oshp << spp;
	spp->xpt = gelon;
	spp->ypt = grlats[i];
	oshp.write((char *)spp, sizeof(polyb));
	//oshp << spp;
	recnum++;

	}//end loop
	//now we have to do it again for the north-south lines
	for(int i = 0; i < xrange; i++){

	//shp
	shprh->recno = SWAP_INT32(recnum); //only 1 record
//shx
	shxr->shp_offset = SWAP_INT32((oshp.tellp()/2)); //offset in words

//dbf

	oshp.write((char *)shprh, sizeof(shp_rec_header));
	oshx.write((char *)shxr, sizeof(shx_rec));
	odbf.write((char *)dbfr, sizeof(dbfxprec));
  //get the poly record ready

//write the poly record header
	oshp.write((char *)spr, sizeof(shp_poly_rec));
	//now populate the points - 2 off each line
	//south to north

	spp->xpt = grlons[i];
	spp->ypt = gslat;
	oshp.write((char *)spp, sizeof(polyb));

	spp->xpt = grlons[i];
	spp->ypt = gnlat;

	oshp.write((char *)spp, sizeof(polyb));

	recnum++;

	}//end loop
	//re-write the headers with correct info
	shph->fil_len = SWAP_INT32(oshp.tellp() / 2);
	oshp.seekp(0, ios::beg);
	oshp.write((char *)shph, sizeof(shp_header));
	shph->fil_len = SWAP_INT32(oshx.tellp() / 2);
	oshx.seekp(0, ios::beg);
	//shph->fil_len = SWAP_INT32(50 + (1 * 4));
	oshx.write((char *)shph, sizeof(shp_header));
	dbfh->reccount = recnum - 1;
	odbf.seekp(0, ios::beg);
	odbf.write((char *)dbfh, sizeof(dbf_header));

	oshp.close();
	oshx.close();
	odbf.close();
//delete all the memory structs
	delete vfh;
	delete vph;
	//delete vpp;
	delete dbfh;
	delete dbffd;
	delete dbfr;
	delete shph;
	delete shprh;
	delete spr;
	delete spp;
	delete shxr;
}
//--------------------------------------------------------------------------
