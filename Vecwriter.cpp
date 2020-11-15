/*
This file will write the polygon out to a .vec file.
This is the same format that XPUtils and XPMesh (and FlyMesh) uses.
 */
//---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <stdio.h>
#include <vector>
#pragma hdrstop
#include "VFCheaders.h"
#include "Vecwriter.h"
#include "shape.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace std;

Vecwriter::Vecwriter()
{
	vmsg = _D("New Vecwriter created");
	Form1->log.writelog(vmsg.c_str());
}
//---------------------------------------------------------------------------
Vecwriter::~Vecwriter()
{
	vmsg = _D("Vecwriter destroyed");
	Form1->log.writelog(vmsg.c_str());
}
//---------------------------------------------------------------------------
void Vecwriter::WriteVectorFile(vector<LatLonPoint> vllp, int numpts)
{
	//write out the polygon to a *.vec file

	String Vecf = filestem + L".vec";

	ofstream ovec(Vecf.c_str(), ios::binary);
	if(!ovec){
		Application->MessageBox(L"Failed to create Shapefile",
								Form1->VCmsg.c_str(), MB_OK);
		return;
	}
	//no bounding box info in the file so the reader will have to make one

	VFC_vector_file_header *vfh = new VFC_vector_file_header;
	VFC_polygon_header *vph = new VFC_polygon_header;
	VFC_poly_point *vpp = new VFC_poly_point;

	vector<LatLonPoint>::iterator vi = vllp.begin();
	strcpy(vfh->tvf_magic, "VFCreator file");
	vfh->numrecs = 1;
	memset(vfh->pad, 0x0, 12);

	ovec.write((char *) vfh, sizeof(VFC_vector_file_header));

	vph->numpts = numpts;
	vph->recno = 0;
	vph->s_type = 5;
	memset(vph->v_icao, NULL, 8);
	memset(vph->v_name_ID, NULL, 8);  //reader can get from filename
	//strcpy(vph->v_feature, Vecfeature.c_str());
	UnicodeToUtf8(vph->v_feature, Vecfeature.c_str(),9);
	UnicodeToUtf8(vph->v_icao, Vecicao.c_str(), 8);
	strcpy(vph->v_apt_type, "Land");
	memset(vph->pad, NULL, 117);
	vph->nbound = nbound;
	vph->sbound = sbound;
	vph->wbound = wbound;
	vph->ebound = ebound;

	ovec.write((char *) vph, sizeof(VFC_polygon_header));

	//now to get the points and write them
	/*
	for(llpi = llp.begin(); llpi < llp.end(); llpi++){
		vecout = FloatToStrF(llpi->lat, ffFixed, 15, 10) + ", " +
					FloatToStrF(llpi->lon, ffFixed, 15, 10);
		UnicodeToUtf8(vecoutc, vecout.c_str(), 80);
		strcat(vecoutc, "\n");
		fputs(vecoutc, otxt);
	}
	*/
	for(vi = vllp.begin(); vi < vllp.end(); vi++){
		vpp->plat = vi->lat;
		vpp->plon = vi->lon;
		ovec.write((char *) vpp, sizeof(VFC_poly_point));
		}

	ovec.close();
	delete vfh;
	delete vph;
	delete vpp;
	vmsg = _D("Vector file created");
	Form1->log.writelog(vmsg.c_str());

	return;   //empty the vector in the calling routine
}
//-------------------------------------------------------------------------
