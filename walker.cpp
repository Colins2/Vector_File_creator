//---------------------------------------------------------------------------
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <cmath>
#include <string.h>

#pragma hdrstop
#include "Vec_creator.h"
#include "VFCheaders.h"
#include "Runway.h"
#include "walker.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern void SplitLine(char *s, vector<string> &split, int numstr);
//--------------------------------------------------------------------------
Walker::Walker()
{
	wmsg = _D("New Walker created");
	Form1->log.writelog(wmsg.c_str());
}
//--------------------------------------------------------------------------
Walker::~Walker()
{
	wmsg = _D("Walker object destroyed");
	Form1->log.writelog(wmsg.c_str());
}
//--------------------------------------------------------------------------
void Walker::Split_csv(char *s, vector<string> &vcsv, int numstr)
{
	//using this function to spilt the CSV file rather than splitline()
int k = strlen(s)+1;
int lp = 0, wc = 0;
char ch;
string word;
vcsv.clear();
do{
	ch = s[lp];
	if(ch != 0x2C && ch != '\n'){
		word += ch;
		lp++;
	}else{
	//reached a space
		vcsv.push_back (word);
		word = "";
		wc++;
		lp++;
		if(ch == '\n')
			return;
		//now find more spaces - shouldn't be any
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
			vcsv.push_back (word);
			break;
			}
		word += ch;
		lp++;

	}while(ch != 0x0a);




}
//--------------------------------------------------------------------------
void Walker::PutShort(ofstream out, short s)
{

	union{
	short ins;
	char cs[2];
	};

	ins = s;
	out.put(cs[1]);
	out.put(cs[0]);
}
//--------------------------------------------------------------------------
void Walker::CreateWalkerTextFile()
{
	//Create a walker-style text file from the data in the .grd file
	//get the grd filename
	wchar_t ginfile[MAXFILE];
	wchar_t walkfile[MAXFILE];
	wchar_t indrv[MAXDRIVE];
	wchar_t inpath[MAXPATH];
	wchar_t infn[MAXFILE];
	wchar_t inext[MAXEXT];

	Form1->OpenDialog1->Title = _D("Select grid file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir; //becaues the file was output before
	Form1->OpenDialog1->Filter = _D("Grid files (*.grd) | *.GRD");
	if(Form1->OpenDialog1->Execute()){
		wcscpy(ginfile, Form1->OpenDialog1->FileName.c_str());
		}else{
		return;
	}
	_wfnsplit(ginfile, indrv, inpath, infn, inext);
	wcscat(infn, _D("_Walk"));
	_wfnmerge(walkfile, indrv, inpath, infn, _D(".csv")); //try a csv file

	ifstream igrd(ginfile, ios::binary);
		if(!igrd){
			Application->MessageBox(L"Could not open grid file",
				Form1->VCmsg.c_str(),
				MB_OK);
		return;
		}
	//open the output file
	ofstream owalk(walkfile); //try text mode
		if(!owalk){
			Application->MessageBox(L"Could not open walk route file",
				Form1->VCmsg.c_str(),
				MB_OK);
				igrd.close();

		return;
		}

	//create some struct variables
	Grd_header *gh = new Grd_header;
	Grd_data *gd = new Grd_data;
	LatLonPoint *llp = new LatLonPoint;
	String wc;
	char walkcoords[50];
	//read the headers
	igrd.read((char *)gh, sizeof(Grd_header));
	igrd.read((char *)gd, sizeof(Grd_data));
	int numrecs = gh->recnum;
	//we can ignore the other data as we just want the coordinates
	owalk << "Latitude, Longitude, Elevation\n";
	for(int i = 0; i < numrecs; i++){
		igrd.read((char *)llp, sizeof(LatLonPoint));
		wc = FloatToStrF(llp->lat, ffFixed, 15, 10) + ", "
			+ FloatToStrF(llp->lon, ffFixed, 15, 10) + ", 0";
		UnicodeToUtf8(walkcoords, wc.c_str(), 50);
		owalk << walkcoords << endl;
	}
	//Perfick :-)

	delete gh;
	delete gd;
	delete llp;
	igrd.close();
	owalk.close();
}
//--------------------------------------------------------------------------
void Walker::ImportWalkerDem()
{
	//read the received text file with elevations and make something
	//out of the data. BIL/HDR set or .bt file? Can't make .hgt because
	//not starting on boundary
	//***************make the input file a .csv*******************
	wchar_t winfile[MAXFILE];
	wchar_t indrv[MAXDRIVE];
	wchar_t inpath[MAXPATH];
	wchar_t infn[MAXFILE];
	wchar_t inext[MAXEXT];

	wchar_t bilfile[MAXFILE]; //standard signed short grid, row major, NW corner
	wchar_t hdrfile[MAXFILE]; //Standard Esri/USGS header
	wchar_t prjfile[MAXFILE]; //standard Esri
	wchar_t grdfile[MAXFILE];
	char inbuf[128];
	vector<string> vcsv;

	Form1->OpenDialog1->Title = _D("Select walk elevations file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Grid elevation files (*.csv) | *.CSV");
	if(Form1->OpenDialog1->Execute()){
		wcscpy(winfile, Form1->OpenDialog1->FileName.c_str());
		}else{
		return;
	}
	_wfnsplit(winfile, indrv, inpath, infn, inext);
	//wcscat(infn, _D("_Walk"));
	_wfnmerge(bilfile, indrv, inpath, infn, _D(".bil")); //try a bil file
	_wfnmerge(hdrfile, indrv, inpath, infn, _D(".hdr"));
	_wfnmerge(prjfile, indrv, inpath, infn, _D(".prj"));


	FILE *edat, *ingrd;
	//wcscpy(grdfile, Form1->aptarea.GridFile.c_str());
	//if(!trcmpgrdfile == "")
	Form1->OpenDialog1->InitialDir = Form1->OutDir.c_str();
	Form1->OpenDialog1->Title = _D("Select grid file for this airport area");
	Form1->OpenDialog1->Filter = _D("Grid files (*.grd) | *.GRD");
	if(Form1->OpenDialog1->Execute()){
		wcscpy(grdfile, Form1->OpenDialog1->FileName.c_str());
			}else{
			return;
		}

	if((ingrd = _wfopen(grdfile, _D("rb")))== NULL){
		  Application->MessageBox(L"Could not open grid data file",
				Form1->VCmsg.c_str(),
				MB_OK);
		return;
		}

	//get the data here
	Grd_header *gh = new Grd_header;
	Grd_data *gd = new Grd_data;

	fread((char *)gh, sizeof(Grd_header), 1, ingrd);
	fread((char *)gd, sizeof(Grd_data), 1, ingrd);
	int eastpix = gd->eastpix;
	int westpix = gd->westpix;
	int northpix = gd->northpix;
	int southpix = gd->southpix;
	int xrange = gd->xrange;
	int yrange = gd->yrange;
	int numrecs = gh->recnum;

	//should be enough, close now
	fclose(ingrd);
	delete gd;
	delete gh;

	if((edat = _wfopen(winfile, _D("r")))== NULL){
		 Application->MessageBox(L"Could not open elevations file",
				Form1->VCmsg.c_str(),
				MB_OK);
		return;
		}


	//open the output files
	ofstream obil(bilfile, ios::binary);
		if(!obil){
			Application->MessageBox(L"Could not create Esri BIL file",
				Form1->VCmsg.c_str(),
				MB_OK);
				fclose(edat);
				return;
			}else{
				wmsg = _D("BIL file created");
				Form1->log.writelog(wmsg.c_str());
			}
	ofstream ohdr(hdrfile); //try text mode
		if(!ohdr){
			Application->MessageBox(L"Could not create Esri HDR file",
				Form1->VCmsg.c_str(),
				MB_OK);
				fclose(edat);
				obil.close();
				return;
			}else{
				wmsg = _D("HDR file created");
				Form1->log.writelog(wmsg.c_str());
			}
   ofstream oprj(prjfile); //try text mode
		if(!oprj){
			Application->MessageBox(L"Could not create Esri PRJ file",
				Form1->VCmsg.c_str(),
				MB_OK);
				fclose(edat);
				obil.close();
				ohdr.close();
				return;
			}else{
				wmsg = _D("PRJ file created");
				Form1->log.writelog(wmsg.c_str());
			}
//all files open
//read the walk text file
//first line is just column headers
//if from google, need to add the first column? or take away if from USGS
	char fileln[80];
	char *endptr;
	double ULY = 0, ULX = 0, elevd = 0;
	fgets(inbuf, 128, edat);
	short elev[numrecs];
	//loop through the values
	//we know how many entries there are but will have to read something for other cases
	//we need to extract UL coordinates from the 1st record
	fgets(inbuf, 128, edat);
	Split_csv(inbuf, vcsv, 3);
	ULY = strtod(vcsv[0].c_str(), &endptr);
	double ULYMAP = ULY + (1/2400);
	ULX = strtod(vcsv[1].c_str(), &endptr);
	double ULXMAP = ULX - (1/2400);
	elevd = strtod(vcsv[2].c_str(), &endptr); //rounding?
	elev[0] = round(elevd);
	//vcsv.clear();

	for(int i = 1; i < numrecs; i++){
		fgets(inbuf, 128, edat);
		Split_csv(inbuf, vcsv, 3);
		//do something with the split[3] elevation
		elevd = strtod(vcsv[2].c_str(), &endptr);
		elev[i] = round(elevd);
		//vcsv.clear();
		}
	union{
		short ins;
		char cs[2];
	};
	//all elevations now short ints in the elev[] array
	for(int i = 0; i < numrecs; i++){
		ins = elev[i];
		obil.put(cs[0]);
		obil.put(cs[1]);
	}
	int nbands = 1;
	int nbits = 16;
	int dwidth = xrange;
	int dheight = yrange;
	double xdim = 0, ydim = 0;
	xdim = (1.0/1200);
	ydim = xdim;
	//write the header file
	//write the lines of text
	memset(fileln, NULL, 80);
	strcpy(fileln, "BYTEORDER      I\n");
	ohdr << fileln;
	strcpy(fileln, "LAYOUT       BIL\n");
	ohdr << fileln;
	sprintf(fileln,"NROWS         %d\n", dheight);
	ohdr << fileln;
	sprintf(fileln,"NCOLS         %d\n", dwidth);
	ohdr << fileln;
	sprintf(fileln,"NBANDS        %d\n",nbands);
	ohdr << fileln;
	sprintf(fileln,"NBITS         %d\n",nbits);
	ohdr << fileln;
	sprintf(fileln,"BANDROWBYTES         %d\n", dwidth*2);
	ohdr << fileln;
	sprintf(fileln,"TOTALROWBYTES         %d\n",dwidth*2);
	ohdr << fileln;
	sprintf(fileln,"BANDGAPBYTES         0\n");
	ohdr << fileln;
	sprintf(fileln,"NODATA        0\n");
	ohdr << fileln;
	sprintf(fileln,"ULXMAP        %0.9f\n",ULXMAP);
	ohdr << fileln;
	sprintf(fileln,"ULYMAP        %0.9f\n",ULYMAP);
	ohdr << fileln;
	sprintf(fileln,"XDIM        %0.9f\n",xdim);
	ohdr << fileln;
	sprintf(fileln,"YDIM        %0.9f\n",ydim);
	ohdr << fileln;

	//prj file
	oprj << "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",";
	oprj << "SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM";
	oprj << "[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]";

		   fclose(edat);
		   ohdr.close();
		   obil.close();
		   oprj.close();

}
//-------------------------------------------------------------------------
