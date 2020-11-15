//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sys/stat.h>
#include <time.h>
#pragma hdrstop

#include "shape.h"
#include "Runway.h"
#include "Vec_creator.h"
#include "logmsg.h"
#include "RCoords.h"
#include "VFCheaders.h"
#include "Shapewriter.h"
#include "Vecwriter.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
using namespace std;
//---------------------------------------------------------------------------
Runway::Runway()
{

}
//--------------------------------------------------------------------------
Runway::~Runway()
{

}
//--------------------------------------------------------------------------
int Runway::LL_Mantissa_check(vector<string> &split, int fn){
//check the length of the lat / long mantissa
//fn is the field number to check
int dotcnt = 0, slen = 0, manlen = 0, bp = 0;
char splitval[50];
slen = strlen(split[fn].c_str());
strcpy(splitval, split[fn].c_str());
	for(int i = 0; i < slen; i++){
		if(splitval[i] == 0x2E){
			dotcnt = i+1; //start counting the mantissa? Need to?
			break;
		}

	}
//manlen = slen - dotcnt?
   return slen-dotcnt;
}
//--------------------------------------------------------------------------
//stringsplitter code
void Runway::SplitLine(char *s, vector<string> &split, int numstr){
int k = strlen(s)+1;
int lp = 0, wc = 0;
char ch;
string word;
split.clear(); //clean it out first

do{
	ch = s[lp];
	if(ch != 0x20 && ch != 0x09 && ch != '\n'){
		word += ch;
		lp++;
	}else{
	//reached a space
		split.push_back (word);
		word = "";
		wc++;
		lp++;
		if(ch == '\n')
			return;
		//now find more spaces
		do{
			ch = s[lp];
			lp++;
			}while(ch == 0x20);
			lp--;
	}//end if-else
	}while(wc < numstr);
	//we get here after numstr words have been found and vectorized
	//we've got the number of fields we need just make a string from the rest
	word = "";
	do{

		ch = s[lp];
		if(ch == 0x0a || ch == 0x0d){
			split.push_back (word);
			break;
			}
		word += ch;
		lp++;

	}while(ch != 0x0a);
}

//---------------------------------------------------------------------------
void Runway::GetAptDatCoords()  //for the future
{
	//This routine gets the coordinates from a project apt.dat file
	Form1->OpenDialog1->Title = _D("Select a project apt.dat file");
	Form1->OpenDialog1->Filter = _D("apt.dat files (apt.dat) | APT.DAT");
	if(Form1->OpenDialog1->Execute()){
		Form1->rway.Infile = Form1->OpenDialog1->FileName;
		}else{
			return;  //cancelled
		}
	FILE *indat;
	if((indat = _wfopen(Infile.c_str(), L"rb")) == NULL){
		Application->MessageBox(L"Could not open file", Form1->VCmsg.c_str(),
								  MB_OK);
		return;
		}
	//read the file
	//routines taken from XPUtils
	vector<string> split;
	char inbuf[128];
	char temp[20];
	char *endptr;
	char icao[8];
	//3 header lines
	fgets(inbuf, 128, indat);
	fgets(inbuf, 128, indat);
	fgets(inbuf, 128, indat);
	//1st line
	fgets(inbuf, 128, indat);
	SplitLine(inbuf, split, 5);
	//only want the name
	//strcpy(temp, split[5].c_str());
	//Form1->rway.AptName = temp;
	Form1->rway.AptName = UTF8ToUnicodeString(split[5].c_str());
	Form1->rway.icao = UTF8ToUnicodeString(split[4].c_str());
	//strcpy(icao, split[4].c_str());
	//UnicodeToUtf8(icao, split[4], 8);
	//maybe utf8 to Unicode?
	//Form1->rway.AptName = UTF8ToUnicodeString(split[5]);

	split.clear();
	//do loop to get lines up to the 100 line
	do{
	  fgets(inbuf, 128, indat);
	  if(!strncmp(inbuf, "100", 3)){
		//runway line
		SplitLine(inbuf, split, 19);
		Form1->rway.width = strtod(split[1].c_str(), &endptr);
		Form1->rway.lolat = strtod(split[9].c_str(), &endptr);
		Form1->rway.lolon = strtod(split[10].c_str(), &endptr);
		Form1->rway.hilat = strtod(split[18].c_str(), &endptr);
		Form1->rway.hilon = strtod(split[19].c_str(), &endptr);
		break;  //got what we need
	  }

	}while(!feof(indat));
	fclose(indat);
	split.clear();

	return;
}
//---------------------------------------------------------------------------
void Runway::GetDirectInput()
{
	//open a small form to get the info
	RwyInput->ShowModal();
	return;
}
//---------------------------------------------------------------------------
void Runway::RwyMain(int source)
{
	switch(source){

	case 1:     //use the form
			//Form1->rway.GetDirectInput();
			GetDirectInput();
			Form1->rway.lolat = RwyInput->LElat->Text.ToDouble();
			Form1->rway.lolon = RwyInput->LElon->Text.ToDouble();
			Form1->rway.hilat = RwyInput->HElat->Text.ToDouble();
			Form1->rway.hilon = RwyInput->HElon->Text.ToDouble();
			Form1->rway.width = RwyInput->Rwidth->Text.ToDouble();
			Form1->rway.AptName = RwyInput->AptName->Text;
			Form1->rway.icao = RwyInput->icao->Text;
			//got the info we need
			break;
	case 2:		//read an apt.dat file and put the values in the rway object
			GetAptDatCoords();
			break;
	default:    //no default
			break;
	}



	LatLonPoint *llpp = new LatLonPoint;
	vector<LatLonPoint> llp;
	vector<LatLonPoint>::iterator llpi = llp.begin();


	//Vecstore::OpenVecStore();
	struct tm *sysdate;
	time_t loctm;
	//calculation variables
	double dlat = 0, dlon = 0, rwyhdg = 0, midlat = 0, dist = 0, dep = 0,
				rwyangle=0;
	bool NE = false, South = false;

	//buitlt-in values for testing
	/*
	Form1->rway.lolat = -6.962742;
	Form1->rway.lolon = 145.642320;
	Form1->rway.hilat = -6.961890;
	Form1->rway.hilon = 145.646987;
	Form1->rway.width = 30.0;
	Form1->rway.setaname(L"Simbari");
	*/

	String filestem = L".\\output\\" + Form1->rway.AptName;
	filestem += "_runway";
	String Atxt = filestem + L".txt";
	//use the AptName as a file stem

	if(Form1->rway.hilat < 0)
		South = true;
	if(Form1->rway.hilat > Form1->rway.lolat)
		NE = true;
	//Got the ends and width
	//Calculate the true heading using plane sailing formulae
	dlat = (diff(Form1->rway.lolat, Form1->rway.hilat)); //degrees
	dlon = (diff(Form1->rway.lolon, Form1->rway.hilon))*60;//minutes
	midlat = (Form1->rway.lolat + Form1->rway.hilat)/2;
	dlat *= 60; //now in minutes
	dep = dlon * cos(midlat / RAD);
	rwyangle = ((atan(dlat / dep)) * RAD);
//------------------------------------------------------------------------
//TODO fix the runway heading - only for display so not urgent
//------------------------------------------------------------------------

	if(NE){
			rwyhdg = 90-rwyangle;
		}else{
			rwyhdg  = rwyangle + 90;
		}
	//runway length - not really needed
	//rwyhdg not needed but good to check
	//dist = abs((cos(rwyangle / RAD)* dep)*1852); //runway length in metres
	dist = abs((dep / cos(rwyangle / RAD))*1852);
	//print out the rwyhdg and rwylength on the log
	rmsg = L"Runway heading is : " + FloatToStrF(rwyhdg, ffFixed, 6, 1) + "° True";
	Form1->log.writelog(rmsg.c_str());
	rmsg = L"Runway length is : " + FloatToStrF(dist, ffFixed, 6, 0) + " Metres";
	Form1->log.writelog(rmsg.c_str());

	//now to get the corner points - calculate the offset
	double rdist = 0, rdlat = 0, rdlon = 0, rdep = 0;
	rdist = (Form1->rway.width / 1852)/2; //1/2 runway width in nm
	rdlat = (rdist * (cos(rwyangle / RAD)) / 60);
	rdep = rdist * (sin(rwyangle / RAD));
	rdlon = rdep * (cos(midlat / RAD)) /60;

		//UL
		llpp->lat = Form1->rway.hilat + rdlat;
		llpp->lon = Form1->rway.hilon - rdlon;
		llp.push_back(*llpp);

		//UR
		llpp->lat = Form1->rway.hilat - rdlat;
		llpp->lon = Form1->rway.hilon + rdlon;
		llp.push_back(*llpp);

		//LR
		llpp->lat = Form1->rway.lolat - rdlat;
		llpp->lon = Form1->rway.lolon + rdlon;
		llp.push_back(*llpp);

		//LL
		llpp->lat = Form1->rway.lolat + rdlat;
		llpp->lon = Form1->rway.lolon - rdlon;
		llp.push_back(*llpp);

		//copy the first point
		llpp->lat = llp[0].lat;
		llpp->lon = llp[0].lon;
		llp.push_back(*llpp);

 //write the points out in a text file - optional later maybe
	 String vecout;
	 char vecoutc[40];
	 FILE *otxt;

	 if((otxt = _wfopen(Atxt.c_str(), L"wb")) == NULL){
		Application->MessageBox(L"Couldn't open file", L"VecCreator", MB_OK);
		delete llpp;
		return;
		}
	//write out the pairs of numbers

	for(llpi = llp.begin(); llpi < llp.end(); llpi++){
		vecout = FloatToStrF(llpi->lat, ffFixed, 15, 10) + ", " +
					FloatToStrF(llpi->lon, ffFixed, 15, 10);
		UnicodeToUtf8(vecoutc, vecout.c_str(), 80);
		strcat(vecoutc, "\n");
		fputs(vecoutc, otxt);
	}

	delete llpp;
	rmsg = L"Runway coordinates written to text file";
	Form1->log.writelog(rmsg.c_str());
	fclose(otxt);
//set up the .vec file and write it
		Vecwriter vecwriter;
		vecwriter.SetVecname(Form1->rway.AptName);
		vecwriter.SetVecfeature(L"Runway");
		vecwriter.SetVecfilestem(filestem);
		vecwriter.SetVecicao(icao);
        //do we need a bounding box? - maybe not in the runway file
		vecwriter.WriteVectorFile(llp, 5);


//set up the shapefile and write it out
		Shpwriter shpwriter;

		shpwriter.SetShpname(Form1->rway.AptName);
		shpwriter.SetShpfeature(L"Runway");
		shpwriter.SetShpfilestem(filestem); //to be modified?

		vector<double> v1;
		vector<double> v2;
		for(int i = 0; i < 4;i++){
			v1.push_back(llp[i].lat);
			v2.push_back(llp[i].lon);
			}
	   //try sorting
	   sort(v1.begin(), v1.end());
	   sort(v2.begin(), v2.end());

	   //direct in shpwriter object
	   shpwriter.nbound = v1[3];
	   shpwriter.sbound = v1[0];
	   shpwriter.wbound = v2[0];
	   shpwriter.ebound = v2[3];

	 shpwriter.Writeshapefile(llp, 5);
	 llp.clear();


	return;    //shpwriter destroyed leaving here
}
//---------------------------------------------------------------------------