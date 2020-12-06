//---------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <dir.h>
#include <fstream>

#pragma hdrstop
#include "Vec_creator.h"
#include "VFCheaders.h"
#include "Apt_area.h"
#include "Dempatch.h"
#include "xpfstructs.h"
#include "DSF_Utils.h"


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
//******TODO***** re-write using streams and Strings or combine with BIL?
void Dempatcher::PatchHgt()
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
void Dempatcher::PatchDSF()
//patch the extracted dsf dem block and re-write the file
//or patch the dsf directly?
{
	String grdfile, patchfile, dsffile;
	long demd_address = 0;

	//get the patch as a BIL file.
	//not big so load into a 1 dimension array
	//need to open the .grd file as well to get the dimensions
	Form1->OpenDialog1->Title = _D(" Select the grid file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir.c_str();
	Form1->OpenDialog1->Filter = _D("Grid files (*.grd) | *.GRD");
	if(Form1->OpenDialog1->Execute()){
		grdfile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	Form1->OpenDialog1->Title = _D(" Select the dsf file to patch");
	Form1->OpenDialog1->InitialDir = Form1->InDir.c_str();
	Form1->OpenDialog1->Filter = _D("dsf files (*.dsf) | *.DSF");
	if(Form1->OpenDialog1->Execute()){
		dsffile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	Form1->OpenDialog1->Title = _D(" Select the patch file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir.c_str();
	Form1->OpenDialog1->Filter = _D("Grid files (*.bil) | *.BIL");
	if(Form1->OpenDialog1->Execute()){
		patchfile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	//read the grid file first
	ifstream ingrd(grdfile.c_str(), ios::binary);
	if(!ingrd){
		Application->MessageBox(_D("Failed opening grid file"),
			Form1->VCmsg.c_str(), MB_OK );
			return;
	}

	ifstream inbil(patchfile.c_str(), ios::binary);
	if(!inbil){
		Application->MessageBox(_D("Failed opening patch file"),
			Form1->VCmsg.c_str(), MB_OK );
			ingrd.close();
			return;
	}

	//open the dsf file for reading to find the address to write to.
	fstream indsf(dsffile.c_str(), ios::in | ios::out | ios::binary);
	if(!inbil){
		Application->MessageBox(_D("Failed opening dsf file"),
			Form1->VCmsg.c_str(), MB_OK );
			ingrd.close();
			inbil.close();
			return;
	}

		// all files open
	//Now start to read the dsf
	auto *dshd = new DSFheader;
	auto *dsat = new DSFatom;
	auto *demr = new DEMIrec;
	auto *grh = new Grd_header;
	auto *grd = new Grd_data;

	long alen = 0;
	char atname[5];
	long indsptr;
	char ch;
	union{
		int ian;
		char can[4];
	};
	indsf.read((char *) dshd, sizeof(DSFheader));
	//check magic?
	if(strncmp(dshd->magic, "XPLNEDSF", 8)){
		Application->MessageBox(_D("X-Plane signature not present."),
			Form1->VCmsg.c_str(), MB_OK);
		indsf.close();
		inbil.close();
		ingrd.close();
		return;
	}
	//now read the atoms
	for(int i = 0; i < 4; i++){
	indsf.read((char *)dsat, sizeof(DSFatom));
	//ian = SWAP_INT32(dsat->ianame);
	//strncpy(atname, can, 4);
	alen = dsat->asize;
	indsf.seekg(alen-8, ios::cur);
	}
	//now at DEMS
	indsf.read((char *)dsat, sizeof(DSFatom));
	//ian = SWAP_INT32(dsat->ianame);
	//strncpy(atname, can, 4);
	alen = dsat->asize; //not interested address of eof before md5
	//read DEMI atom
	indsf.read((char *)dsat, sizeof(DSFatom));
	//nothing to do with this info
	indsf.read((char *)demr, sizeof(DEMIrec));
	//check stuff?
	indsf.read((char *)dsat, sizeof(DSFatom)); //DEMD
	//store this address for re-writing?
	demd_address = indsf.tellg();
	//try to open read-write instead - done!
	indsf.seekp(indsf.tellg());
	//put pointer at write position for data
	//read the grd file data
	ingrd.read((char *)grh, sizeof(Grd_header));
	ingrd.read((char *)grd, sizeof(Grd_data));
	int dsfrow = grd->southpix;
	int dsfcol = grd->westpix;
	int pcols = grd->xrange;
	int prows = grd->yrange;
	/*
	short **demblk;
	demblk = new short* [rows];
	for(int i = 0; i < rows; i++){
		demblk[i] = new short [cols];
	}
	*/
	//due to the order of the data values we will have to use a 2d array
	short **patchval;
	patchval = new short* [grd->yrange];
		for(int i = 0; i < grd->yrange; i++){
			patchval[i] = new short [grd->xrange];
		}
	for(int i = 0; i < grd->yrange; i++){
		for(int j = 0; j < grd->xrange; j++){
			patchval[i][j] = Dsffile::sgetsh(inbil);
		}
	}

	//patchval block is top-down

	//The DSF dem data block is written bottom up i.e. from the SW corner
	//instead of BIL style or HGT from NW downwards

	int pc = 0; //patchval count
	//the  dsf data block is effectively a continuous stream of points
	//so we have to count down 1201 (wide) * northpix + west pix to get to
	//the first point to replace.
	int dsfpoint = ((dsfrow * 1201) + dsfcol)*2;
	indsf.seekp(dsfpoint, ios::cur);
	for(int i = prows-1; i >= 0; i--){
		for(int j = 0; j < pcols; j++){
			Dsffile::sputsh(indsf, patchval[i][j]);
			//the file pointer will increment itself
			pc++;
		}
		indsf.seekp((1201-pcols)*2, ios::cur);
	}
	//that should be it. it's written to the dsf directly, so how to check?
	//could check the same method using the dsf .bil file in the patchdem function

	//delete and close stuff
	delete dshd;
	delete dsat;
	delete demr;
    for(int i = 0; i < grd->yrange; i++){
		delete patchval[i];
	}
	delete [] patchval;
	delete grh;
	delete grd;


	indsf.close();
	inbil.close();
	ingrd.close();

}
//----------------------------------------------------------------------------
void Dempatcher::PatchBIL()
	/*This function is more or less the same as patching the HGT file apart from
	  the fact that you are working with a generated file that can easily be
	  made again if it is not correct. Patching the HGT file wrongly means you
	  will have to download it again (or get it from your downloads).
	  Similarly, patching the DSF directly is non-reversable. I don't see the
	  need for the extra code.
	  Patching the BIL file, which is derived from the DSF, will allow easy
	  checking in (say) QGis with the runway and airport area polygons showing
	  as well
	  It will overwrite the existing 'tilename'.BIL
	*/
{
String grdfile, patchfile, bilfile;
	long demd_address = 0;

	//get the patch as a BIL file.
	//not big so load into a 1 dimension array
	//need to open the .grd file as well to get the dimensions
	Form1->OpenDialog1->Title = _D(" Select the grid file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir.c_str();
	Form1->OpenDialog1->Filter = _D("Grid files (*.grd) | *.GRD");
	if(Form1->OpenDialog1->Execute()){
		grdfile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	Form1->OpenDialog1->Title = _D(" Select the (dsf)BIL file to patch");
	Form1->OpenDialog1->InitialDir = Form1->InDir.c_str();
	Form1->OpenDialog1->Filter = _D("BIL files (*.bil) | *.BIL");
	if(Form1->OpenDialog1->Execute()){
		bilfile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	int retval = Application->MessageBox(_D("****WARNING******\n"
			"This file will be overwritten if you proceed"),
			Form1->VCmsg.c_str(), MB_OKCANCEL);

	//if(retval == 6)//cancel
	//	return; //nothing to close


	Form1->OpenDialog1->Title = _D(" Select the patch file");
	Form1->OpenDialog1->InitialDir = Form1->OutDir.c_str();
	Form1->OpenDialog1->Filter = _D("Grid files (*.bil) | *.BIL");
	if(Form1->OpenDialog1->Execute()){
		patchfile = Form1->OpenDialog1->FileName.c_str();
		}else{
		return;
	}

	//read the grid file first
	ifstream ingrd(grdfile.c_str(), ios::binary);
	if(!ingrd){
		Application->MessageBox(_D("Failed opening grid file"),
			Form1->VCmsg.c_str(), MB_OK );
			return;
	}

	ifstream inbil(patchfile.c_str(), ios::binary);
	if(!inbil){
		Application->MessageBox(_D("Failed opening patch file"),
			Form1->VCmsg.c_str(), MB_OK );
			ingrd.close();
			return;
	}

	//open the tilename.bil file for reading to find the address to write to.
	fstream outbil(bilfile.c_str(), ios::in | ios::out | ios::binary);
	if(!outbil){
		Application->MessageBox(_D("Failed opening bil file"),
			Form1->VCmsg.c_str(), MB_OK );
			ingrd.close();
			inbil.close();
			return;
	}

		// all files open

	auto *grh = new Grd_header;
	auto *grd = new Grd_data;

	//read the grd file data
	ingrd.read((char *)grh, sizeof(Grd_header));
	ingrd.read((char *)grd, sizeof(Grd_data));
	auto *patchval = new short[grh->recnum];
	//read the data from the inbil file
	for(int i = 0; i < grh->recnum; i++){
		patchval[i] = Dsffile::sgetsh(inbil);
	}
	//all values read into a continuous buffer
	int dsfcol = grd->westpix;
	int dsfrow = 1201 - grd->northpix + 1;
	//should be the NW corner
	int pcols = grd->xrange;
	int prows = grd->yrange;
	int pc = 0; //patchval count
	//the  dsf data block is effectively a continuous stream of points
	//so we have to count down 1201 (wide) * northpix + west pix to get to
	//the first point to replace.
	int dsfpoint = ((dsfrow * 1201) + dsfcol)*2; //2 bytes per point
	outbil.seekp(dsfpoint, ios::beg); //start from the beginning as this is a BIL file
	for(int i = 0; i < prows; i++){
		for(int j = 0; j < pcols; j++){
			Dsffile::sputsh(outbil, patchval[pc]);
			//the file pointer will increment itself
			pc++;
		}
		outbil.seekp((1201-pcols)*2, ios::cur);  //next row
	}

	//delete and close stuff
	delete grh;
	delete grd;
	delete [] patchval;

	outbil.close();
	inbil.close();
	ingrd.close();

}
//---------------------------------------------------------------------------



