//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>

#pragma hdrstop
#include "Vec_creator.h"
#include "VFCheaders.h"
#include "Apt_area.h"
#include "Dempatch.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

//---------------------------------------------------------------------------
Dempatcher::Dempatcher()
{
	dmsg = _D("Dempatcher created");
	Form1->log.writelog(dmsg.c_str());
}
//---------------------------------------------------------------------------
Dempatcher::~Dempatcher()
{
	dmsg = _D("Dempatcher destroyed");
	Form1->log.writelog(dmsg.c_str());
}
//---------------------------------------------------------------------------
void Dempatcher::fputsh(FILE *of, short sh)
{
	union{
		short ins;
		char ochar[2];
	};
	ins = sh;
	fputc(ochar[0], of);
	fputc(ochar[1], of);

}
//--------------------------------------------------------------------------
short Dempatcher::fgetsh(FILE *inf)
{
	union{
		char inch[2];
		short osh;
	};
	inch[0] = fgetc(inf);
	inch[1] = fgetc(inf);

	return osh;
}
//--------------------------------------------------------------------------
short Dempatcher::fgetles(FILE *inf)
{
	union{
		char inch[2];
		short osh;
	};
	inch[1] = fgetc(inf);
	inch[0] = fgetc(inf);

	return osh;
}
//------------------------------------------------------------------------
void Dempatcher::fputles(FILE *of, short sh)
{
	union{
		short ins;
		char ochar[2];
	};
	ins = sh;
	fputc(ochar[1], of);
	fputc(ochar[0], of);

}
//--------------------------------------------------------------------------
void Dempatcher::PatchDem()
{
	//should be able to do most of it in 1 function but if using FILE
	//then we can use s function call for reading / writing short ints
	//get the patch
	//first check the pixel size and get the ULX / ULY coordinates
	//these coordinates are the outside corner of the pixel
	//calculate the grid positions?
	//input patch files
	FILE *inhgt, *inbil, *inhdr, *ingrd;

	wchar_t bilfile[MAXFILE];
	wchar_t hdrfile[MAXFILE];
	wchar_t hgtfname[MAXFILE];//needed?
	//input dem file(s) if HGT, only 1 file
	wchar_t indemfile[MAXFILE];
	//ASSUME THAT IT WILL ALWAYS BE an HGT file
	wchar_t indrv[MAXDRIVE];
	wchar_t inpath[MAXPATH];
	wchar_t infn[MAXFILE];
	wchar_t inext[MAXEXT];
	wchar_t outhgt[MAXFILE];
	wchar_t ingrid[MAXFILE];
	int hgtsize = 0;
	//get the grd file
	Form1->OpenDialog1->Title = _D("Select the airport grid file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Airport grid files (*.grd) | *.GRD");
	if (Form1->OpenDialog1->Execute()) {
		wcscpy(ingrid, Form1->OpenDialog1->FileName.c_str());
		}else{
		return;
	}

	//get the hgt file - assume it is in the InDir
	Form1->OpenDialog1->Title = _D("Select tile dem file");
	Form1->OpenDialog1->InitialDir = Form1->InDir;
	Form1->OpenDialog1->Filter = _D("SRTM HGT files (*.hgt) | *.HGT");
	if (Form1->OpenDialog1->Execute()) {
		wcscpy(indemfile, Form1->OpenDialog1->FileName.c_str());
		}else{
		return;
	}

	if((inhgt = _wfopen(indemfile, _D("rb")))== NULL){
		Application->MessageBox(_D("Failed to open dem file"),
			Form1->VCmsg.c_str(), MB_OK);
			fclose(ingrd);
			return;
	}
	fseek(inhgt, 0, SEEK_END);
	hgtsize = ftell(inhgt);
	if(hgtsize != 2884802){
		Application->MessageBox(_D("This dem file is not 1201 x 1201"),
			Form1->VCmsg.c_str(), MB_OK);
			fclose(inhgt);
			fclose(ingrd);
			return;
	}
	fseek(inhgt, 0, SEEK_SET); //back to the start
	//get the hgt filename
	_wfnsplit(indemfile, indrv, inpath, infn, inext);
	wcscpy(hgtfname, infn);
	//Openfile dialog to find BIL
	Form1->OpenDialog1->Title = _D("Select dem patch file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir;
	Form1->OpenDialog1->Filter = _D("Esri BIL files (*.bil) | *.BIL");
	if (Form1->OpenDialog1->Execute()) {
		wcscpy(bilfile, Form1->OpenDialog1->FileName.c_str());
		}else{
		return;
	}
	//split the bilfile to get the .hdr filename
	_wfnsplit(bilfile, indrv, inpath, infn, inext);
	_wfnmerge(hdrfile, indrv, inpath, infn, _D(".hdr"));
	_wfnmerge(outhgt, indrv, inpath, hgtfname, _D(".hgt"));

	vector<string> split;
	double ULXMAP = 0, ULYMAP = 0, xdim = 0, ydim = 0;
	char inbuf[128], *endptr;;
	int nrows = 0, ncols = 0;

	//open the .grd file to get the array sizes
	if((ingrd = _wfopen(ingrid, _D("rb")))== NULL){
		Application->MessageBox(_D("Failed to open grid file"),
			Form1->VCmsg.c_str(), MB_OK);
			return;
	}

	dmsg = _D("Reading the grid file");
	Form1->log.writelog(dmsg.c_str());

	//read the grd file to get dimension info
	Grd_header *gh = new Grd_header;
	Grd_data *gd = new Grd_data;
	fread((char *)gh, sizeof(Grd_header), 1, ingrd);
	fread((char *)gd, sizeof(Grd_data), 1, ingrd);
	fclose(ingrd);
	int westpix = gd->westpix;
	int southpix = gd->southpix;
	int northpix = gd->northpix; //counted from SW corner
	northpix = 1201-northpix+1;
	int eastpix = gd->eastpix;
	int recnum = gh->recnum;

	//delete the dynamic variables
	delete gd;
	delete gh;

	//open the header and bil files
	if((inhdr = _wfopen(hdrfile, _D("r")))==NULL){
		Application->MessageBox(_D("Failed to open hdr file"),
			Form1->VCmsg.c_str(), MB_OK);
			fclose(inhgt);
			fclose(ingrd);
			return;
	}

	if((inbil = _wfopen(bilfile, _D("rb")))== NULL){
		Application->MessageBox(_D("Failed to open bil file"),
			Form1->VCmsg.c_str(), MB_OK);
			fclose(inhgt);
			fclose(inhdr);
			fclose(ingrd);
			return;
	}

	dmsg = _D("Reading the header file");
	Form1->log.writelog(dmsg.c_str());
	//now read the header file
	//we know the values are good because we wrote the file!

	fgets(inbuf, 128, inhdr); //byteorder
	fgets(inbuf, 128, inhdr); //layout
	fgets(inbuf, 128, inhdr); //nrows
	Form1->rway.SplitLine(inbuf, split, 2);
	nrows = strtol(split[1].c_str(), &endptr, 10);
	fgets(inbuf, 128, inhdr); //ncols
	Form1->rway.SplitLine(inbuf, split, 2);
	ncols = strtol(split[1].c_str(), &endptr, 10);
	fgets(inbuf, 128, inhdr); //nbands
	fgets(inbuf, 128, inhdr); //nbits
	fgets(inbuf, 128, inhdr); //bandrowbytes
	fgets(inbuf, 128, inhdr); //totalrowbytes
	fgets(inbuf, 128, inhdr); //bandgapbytes
	fgets(inbuf, 128, inhdr); //nodata value
	fgets(inbuf, 128, inhdr); //ULXMAP
	Form1->rway.SplitLine(inbuf, split, 2);
	ULXMAP = strtod(split[1].c_str(), &endptr);
	fgets(inbuf, 128, inhdr); //ULYMAP
	Form1->rway.SplitLine(inbuf, split, 2);
	ULXMAP = strtod(split[1].c_str(), &endptr);
	fgets(inbuf, 128, inhdr); //xdim    pixel size
	Form1->rway.SplitLine(inbuf, split, 2);
	xdim = strtod(split[1].c_str(), &endptr);
	fgets(inbuf, 128, inhdr); //ydim
	Form1->rway.SplitLine(inbuf, split, 2);
	ydim = strtod(split[1].c_str(), &endptr);
	//end of file
	fclose(inhdr); //finished with it

	//make a dynamic array ncols x nrows and read the .bil file into it
	short **bilblk;
	bilblk = new short* [nrows];   //latitudes
	for(int i = 0; i < nrows; i++){
		bilblk[i] = new short [ncols];       //longitudes
	}
	//read the data row by row
	for(int i = 0; i < nrows; i++){
		for(int j = 0; j < ncols; j++) {
			bilblk[i][j] = fgetsh(inbil);
		}
	}

	//close the bil file
	fclose(inbil);
	//make a dynamic array 1201 x 1201 and read the hgt file in
	short **hgtblk;
	hgtblk = new short* [1201];
	for(int i = 0; i < 1201; i++){
		hgtblk[i] = new short [1201];
	}

	for (int i = 0; i < 1201; i++) {
		for (int j = 0; j < 1201; j++) {
			hgtblk[i][j] = fgetles(inhgt);
		}
	}
	//close the hgt file for reading
	fclose(inhgt);
	//patch the hgt with the bil data
	short old = 0, snew = 0;
	for(int i = 0; i < nrows; i++){
		for(int j = 0; j < ncols; j++){
			//old = hgtblk[northpix+i][westpix+j];
			//snew = bilblk[i][j];
			hgtblk[northpix+i][westpix+j] = bilblk[i][j];
		}
	}

	//write out the hgt agaim as the same filename but in the output folder
	//we can load the hgt file into QGis to check
	//open hgt for writing
	FILE *ohgt;
	if((ohgt = _wfopen(outhgt, _D("wb")))== NULL){
		Application->MessageBox(_D("Failed to create hgt file"),
			Form1->VCmsg.c_str(), MB_OK);
	}else{
		//file opened, write the data
		for(int i = 0; i < 1201; i++){
			for(int j = 0; j < 1201; j++){
				fputles(ohgt, hgtblk[i][j]);
			}
		}
		fclose(ohgt);
	}

	//delete all dynamic arrays
	for(int i = 0; i < ncols; i++){
		delete[] bilblk[i];
	}
	delete [] bilblk;
	for(int i = 0; i < 1201; i++){
		delete[] hgtblk[i];
	}
	delete [] hgtblk;

	//should be done!
}   //exit
//---------------------------------------------------------------------------