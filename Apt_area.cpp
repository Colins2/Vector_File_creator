//---------------------------------------------------------------------------
// This source file will cover airport areas imported from KML or just a
//csv text file created by the user with a couple of heading lines
//It will collect the data and write to a vector file of lat/long points
//which will be accessible from other parts of the program.

#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <dir.h>

#pragma hdrstop
#include "Vec_creator.h"
#include "Shapewriter.h"
#include "VFCheaders.h"
#include "logmsg.h"
#include "Vecwriter.h"

#include "Apt_area.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

/*
Read a KML file and make a shapefile from it the same as the runway function
*/
//---------------------------------------------------------------------------
Aptarea::Aptarea()
{
	//amsg = L"New AptArea created";
	//Form1->log.writelog(amsg.c_str());
}
//---------------------------------------------------------------------------
Aptarea::~Aptarea()
{
	//amsg = L"New AptArea created";
	//Form1->log.writelog(amsg.c_str());
}
//---------------------------------------------------------------------------
void Aptarea::ltrim(char *s)
{
char temp[1024]; //just in case we have a long string in the 256 char inbuf
char ch;
int bp = 0, sp = 0;
strcpy(temp, s);
	for(bp = 0; bp < strlen(temp); bp++){
		ch = temp[bp];
			if (ch == 0x20 || ch == 0x09)
			//could probably say < 0x21 here?
			continue;
		break;
	}
	//now we know where the ascii starts
	//clean out s
	//strset(s, 0x0);
	memset(s, 0x0, strlen(s));
	for (int i = bp; i < strlen(temp); i++) {
		s[sp] = temp[i];
		sp++;
	}
}
//---------------------------------------------------------------------------
void Aptarea::Aamain()
{
	//main source file
//read the kml file like the test app.
//first we'll create a shapefile directly and later maybe write a .vec file

	Form1->OpenDialog1->InitialDir = Form1->InDir;
	Form1->OpenDialog1->Title = _D("Select a KML file");
	Form1->OpenDialog1->Filter = _D("KML files (*.kml) | *.KML");
	if(Form1->OpenDialog1->Execute()){
		Inkml = Form1->OpenDialog1->FileName;
		}else{
			return;  //cancelled
		}
	FILE *indat;
	if((indat = _wfopen(Inkml.c_str(), L"rb")) == NULL){
		Application->MessageBox(L"Could not open file", Form1->VCmsg.c_str(),
								  MB_OK);
		return;
		}
	LatLonPoint allp;
	vector<LatLonPoint> avec;
	vector<LatLonPoint>::iterator avi = avec.begin();
	char inbuf[1024], ch, *endptr;
	char wanted[] = {"<coordinates>"};
	int wl = strlen(wanted);
	char veclen[20];
	char ppoint[24];
	int commas[3];
	int comma_cnt = 0;
	int space_cnt = 0;
	int pcnt = 0, bp = 0, lno = 0, pal = 0;
	int spaces[25];
	wchar_t infile[MAXFILE];
	wchar_t indrv[MAXDRIVE];
	wchar_t inpath[MAXPATH];
	wchar_t infn[MAXFILE];
	wchar_t inext[MAXEXT];
	wcscpy(infile, Inkml.c_str());
	_wfnsplit(infile, indrv, inpath, infn, inext);
	// make the filenames
	String filestem = Form1->OutDir + "\\";
	filestem += infn;
	Form1->AptName = infn;
	Atxtfile = filestem + _D(".txt");
	Shapefile = filestem + _D(".shp");
	Shxfile = filestem + _D(".shx");
	DBFfile = filestem + _D(".dbf");


	do{
		fgets(inbuf, 256, indat);
		ltrim(inbuf);
		if(!strncmp(inbuf, wanted, wl))
			break;
	}while(strncmp(inbuf, wanted, wl));

	fgets(inbuf, 1024, indat);
	fclose(indat);
	ltrim(inbuf);
	for(int i = 0; i < strlen(inbuf); i++){
		ch = inbuf[i];
			if(ch == 0x20){
				space_cnt++;
				spaces[space_cnt] = i;
			}
	}

	amsg = L"Found " + IntToStr(space_cnt) + " coordinates";
	Form1->log.writelog(amsg.c_str());

	for(int i = 0; i < space_cnt; i++){
		//loop through the points
		do{
			ch = inbuf[bp];
			bp++;
			if(ch == ','){ //end of a point
				allp.lon = strtod(ppoint, &endptr);
				pcnt = 0;
				memset(ppoint, NULL, strlen(ppoint));
				break;
			}
			ppoint[pcnt] = ch;
			pcnt++;
		}while(ch != ',');
		//end of first part of coordinate
		pcnt = 0;
		do{
			ch = inbuf[bp];
			bp++;
			if(ch == ','){ //end of a point
				allp.lat = strtod(ppoint, &endptr);
				pcnt = 0;
				memset(ppoint, NULL, strlen(ppoint));
				break;
			}
			ppoint[pcnt] = ch;
			pcnt++;
		}while(ch != ',');
		pcnt = 0;
		avec.push_back(allp);
		bp +=2; //to skip over the 0 and space
	}
	//got all the points in a vector now
	int vl = avec.size();
	//write the coordinates out to a text file the same as for runway.
	String vecout;
	char vecoutc[40];
	FILE *otxt;

	 if((otxt = _wfopen(Atxtfile.c_str(), L"wb")) == NULL){
		Application->MessageBox(L"Couldn't open file", L"VecCreator", MB_OK);
		//delete llpp;
		return;
		}
	//write out the pairs of numbers

	for(avi = avec.begin(); avi < avec.end(); avi++){
		vecout = FloatToStrF(avi->lat, ffFixed, 15, 10) + ", " +
					FloatToStrF(avi->lon, ffFixed, 15, 10);
		UnicodeToUtf8(vecoutc, vecout.c_str(), 80);
		strcat(vecoutc, "\n");
		fputs(vecoutc, otxt);
	}

	amsg = L"Airport area coordinates written to text file";
	Form1->log.writelog(amsg.c_str());
	fclose(otxt);
	//find the boundaries first
	//sort the coordinates to find bounding box
	vector<double> v1;
	vector<double> v2;
	for(int i = 0; i < avec.size()-1; i++){
			v1.push_back(avec[i].lat);
			v2.push_back(avec[i].lon);
			}
	   //try sorting
	   sort(v1.begin(), v1.end());
	   sort(v2.begin(), v2.end());
	   int v1s = 0, v2s = 0;
	   v1s = v2s = v1.size()-1;//both the same size

		Vecwriter vecwriter;

		vecwriter.SetVecname(Form1->AptName);
		vecwriter.SetVecicao(Form1->Aicao); //can be blank
		vecwriter.SetVecfeature(L"Apt_Area");
		vecwriter.SetVecfilestem(filestem);
		vecwriter.nbound = v1[v1s];
		vecwriter.sbound = v1[0];
		vecwriter.wbound = v2[0];
		vecwriter.ebound = v2[v2s];

		vecwriter.WriteVectorFile(avec, vl);

		Shpwriter shpwriter;

		shpwriter.SetShpname(infn);
		shpwriter.SetShpfeature(L"Airport Area");
		shpwriter.SetShpfilestem(filestem); //to be modified?

	//write the values to the shpwriter object
	   shpwriter.nbound = v1[v1s];
	   shpwriter.sbound = v1[0];
	   shpwriter.wbound = v2[0];
	   shpwriter.ebound = v2[v2s];

	 shpwriter.Writeshapefile(avec, vl); //(vector and number of points in record)
	 //clear the vector
	 avec.clear();

	return;
}
//--------------------------------------------------------------------------
