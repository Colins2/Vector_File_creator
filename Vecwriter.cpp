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

}
//---------------------------------------------------------------------------
Vecwriter::~Vecwriter()
{

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
	memset(vph->v_name_ID, NULL, 24);
	memset(vph->v_tile, NULL, 8);
	memset(vph->v_feature, NULL, 9);

	//strcpy(vph->v_feature, Vecfeature.c_str());
	UnicodeToUtf8(vph->v_feature, Vecfeature.c_str(),9);
	UnicodeToUtf8(vph->v_icao, Vecicao.c_str(), 8);
	UnicodeToUtf8(vph->v_name_ID, Vecname.c_str(), 24);
	UnicodeToUtf8(vph->v_tile, Vectile.c_str(), 8);
	strcpy(vph->v_apt_type, "Land");
	vph->v_rwidth = Getrwidth();
	memset(vph->pad, NULL, 85);
	vph->nbound = Getnbound();
	vph->sbound = Getsbound();
	vph->wbound = Getwbound();
	vph->ebound = Getebound();

	ovec.write((char *) vph, sizeof(VFC_polygon_header));

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
