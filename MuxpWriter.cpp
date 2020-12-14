//---------------------------------------------------------------------------
/*
	Create a muxp compatible input file containing the airport area and runway
	spline coordinates We can use the bounding box for the area and then the
	airport area as the cut polygon using -37268 as the value so that muxp
	will take the already modified dsf values.
*/
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
#include <vector>
#include <dir.h>
#include <System.IOUtils.hpp>

#pragma hdrstop

#include "MuxpWriter.h"
#include "logmsg.h"
#include "Vec_creator.h"
#include "VFCheaders.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

//---------------------------------------------------------------------------
MuxpWriter::MuxpWriter()
{
	//init
}
//---------------------------------------------------------------------------
MuxpWriter::~MuxpWriter()
{
	//destructor
}
//---------------------------------------------------------------------------
void MuxpWriter::ReadAptArea()
{
	//get what we need from the AirportArea .vec file
	String aafile;
	//String Aptname;
	char cafile[MAXFILE];   //std::string
	Form1->OpenDialog1->Title = _D("Select Airport Area vector file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Vector files (*.vec) | *.VEC");
	if(Form1->OpenDialog1->Execute())
		aafile = Form1->OpenDialog1->FileName;
	fmuxp = ChangeFileExt(aafile, _D(".muxp"));
	//Aptname = TPath::GetFileNameWithoutExtension(aafile);
	//Apname.assign(Aptname.c_str());
	ifstream inaa(aafile.c_str(), ios::binary);
	if(!inaa){
		Application->MessageBox(_D("Could not open Vector file"),
			Form1->VCmsg.c_str(), MB_OK);
		return;
	}
	VFC_vector_file_header *vfh = new VFC_vector_file_header;
	VFC_polygon_header *vph = new VFC_polygon_header;
	VFC_poly_point *vpp = new VFC_poly_point;

	inaa.read((char *)vfh, sizeof(VFC_vector_file_header));
	inaa.read((char *)vph, sizeof(VFC_polygon_header));
	nbound = vph->nbound;
	sbound = vph->sbound;
	ebound = vph->ebound;
	wbound = vph->wbound;
	Aicao = vph->v_icao;
	//read the points of the airport area.
	for(int i = 0; i < vph->numpts; i++){
		inaa.read((char *)vpp, sizeof(VFC_poly_point));
		Abound.push_back(*vpp);
	}
	//make the tile name
	int slat = (int)sbound;
	int wlon = (int)wbound;
	char tile[8];
	char clon[5];
	//tile.assign(to_string(slat));
	//tile.append(to_string(wlon));
	sprintf(tile, "%+03d", slat);
	sprintf(clon, "%+04d", wlon);
	strcat(tile, clon);
	Atile.assign(tile);
	inaa.close();
	delete vfh;
	delete vph;
	delete vpp;
}
//---------------------------------------------------------------------------
void MuxpWriter::ReadRunwayProfile()
{
	//get the runway profile from it's csv file with elevations
	String rwfile, rcsvfile;
	Form1->OpenDialog1->Title = _D("Select runway vector file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Vector files (*.vec) | *.VEC");
	if(Form1->OpenDialog1->Execute())
		rwfile = Form1->OpenDialog1->FileName;
	ifstream inrw(rwfile.c_str(), ios::binary);
	if(!inrw){
		Application->MessageBox(_D("Could not open Vector file"),
			Form1->VCmsg.c_str(), MB_OK);
		return;
	}
    VFC_vector_file_header *vfh = new VFC_vector_file_header;
	VFC_polygon_header *vph = new VFC_polygon_header;
	//only need the runway width!
	inrw.read((char *)vfh, sizeof(VFC_vector_file_header));
	inrw.read((char *)vph, sizeof(VFC_polygon_header));
	rwidth = vph->v_rwidth;
	Form1->Aicao = vph->v_icao;
	Form1->TileName = vph->v_tile;
	Form1->AptName = vph->v_name_ID;
	delete vph;
	delete vfh;
	inrw.close();

	mxmsg = _D("Runway width read");
	Form1->log.writelog(mxmsg.c_str());

	Form1->OpenDialog1->Title = _D("Select runway profile file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Runway CSV files (*.csv) | *.CSV");
	if(Form1->OpenDialog1->Execute())
		rcsvfile = Form1->OpenDialog1->FileName;
	ifstream inrp(rcsvfile.c_str());
	if(!inrp){
		Application->MessageBox(_D("Could not open profile file"),
			Form1->VCmsg.c_str(), MB_OK);
		return;
	}
	rwaydata rd;
	string S, T, U, V;
	getline(inrp, S); //will be column headings
	while(getline(inrp, S)){
		stringstream X(S);
		getline(X, T, ',');
		getline(X, U, ',');
		getline(X, V, ',');
		strcpy(rd.rlat, T.c_str());
		strcpy(rd.rlon, U.c_str());
		strcpy(rd.relev, V.c_str());
		rdat.push_back(rd);
		}

   inrp.close();
}
//--------------------------------------------------------------------------
void MuxpWriter::WriteMuxp()
{
	string avalue;
	char mtemp[24];
	UnicodeToUtf8(mtemp, Form1->Aicao.c_str(), 24);
	string m1;
	m1.assign(mtemp);
	UnicodeToUtf8(mtemp, Form1->AptName.c_str(), 24);
	string m2;
	m2.assign(mtemp);
	UnicodeToUtf8(mtemp, Form1->TileName.c_str(), 24);
	string m3;
	m3.assign(mtemp);
	ofstream omuxp(fmuxp.c_str());//text file
	if(!omuxp){
		Application->MessageBox(_D("Could not create muxp file"),
			Form1->VCmsg.c_str(), MB_OK);
		return;
	}
	omuxp << "muxp_version: 0.1" << endl;
	omuxp << "id: " << "airport_icao_" << m1 << endl;
	omuxp << "version: 1.0" << endl;
	omuxp << "description: creating TIN for " << m2 << endl;
	omuxp << "author: muxp_auto_creation_from_VFC" << endl;
	omuxp << "tile: " << m3 << endl;
	omuxp << "area: "; // << endl;
	omuxp << to_string(sbound);
	omuxp << " " << to_string(nbound);
	omuxp << " " << to_string(wbound);
	omuxp << " " << to_string(ebound) << endl;
	omuxp << "source_dsf: DEFAULT" << endl << endl;

	omuxp << "limit_edges.Airport_Area:" << endl;
	omuxp << "   edge_limit: 100" << endl;
	omuxp << "   coordinates:" << endl;
	for(int i = 0; i < Abound.size(); i++){
		omuxp << "\t- " << to_string(Abound[i].plat);
		omuxp << " " << to_string(Abound[i].plon) << endl;
	}
	omuxp << endl;

	omuxp << "cut_polygon.airport:" << endl;
	omuxp << "   name: clear_airport_to_base_mesh" << endl;
	omuxp << "   elevation: -37268" << endl;
	omuxp << "   coordinates:" << endl;
	for(int i = 0; i < Abound.size(); i++){
		omuxp << "\t- " << to_string(Abound[i].plat);
		omuxp << " " << to_string(Abound[i].plon) << endl;
	}
	omuxp << endl;

	omuxp << "cut_spline_segment.runway_0:" << endl;
	omuxp << "   width: " << to_string(rwidth) << endl;
	omuxp << "   profile_interval: 20" << endl;
	omuxp << "   terrain: lib/g10/terrain10/apt_tmp_dry.ter" << endl;
	omuxp << "   3d_coordinates:" << endl;
	for(int i = 0; i < rdat.size(); i++){
		omuxp << "\t- " << rdat[i].rlat;
		omuxp << " " << rdat[i].rlon;
		omuxp << " " << rdat[i].relev << endl;
	}

	mxmsg = _D("muxp file created");
	Form1->log.writelog(mxmsg.c_str());
	omuxp.close();
}
//--------------------------------------------------------------------------
void MuxpWriter::MuxpMain()
{

	ReadAptArea();
	ReadRunwayProfile();
	WriteMuxp();

}
//--------------------------------------------------------------------------