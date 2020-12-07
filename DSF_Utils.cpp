//---------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dir.h>
#include <fstream>


#pragma hdrstop

#include "DSF_Utils.h"
#include "logmsg.h"
#include "Vec_creator.h"
#include "VFCheaders.h"
#include "xpfstructs.h"
#include "Dempatch.h"

#pragma package(smart_init)

using namespace std;


//---------------------------------------------------------------------------
Dsffile::Dsffile()
{
	dsmsg = _D("New Dsffile object created");
	Form1->log.writelog(dsmsg.c_str());
}
//--------------------------------------------------------------------------
Dsffile::~Dsffile()
{
	dsmsg = _D("Dsffile object destroyed");
	Form1->log.writelog(dsmsg.c_str());
}
//---------------------------------------------------------------------------
void Dsffile::ExtractFarDem()
{
	//maybe not needed

}
//--------------------------------------------------------------------------
short Dsffile::sgetsh(ifstream& ind)
{
	union{
		char cs[2];
		short ss;
	};
	cs[0] = ind.get();
	cs[1] = ind.get();
	return ss;
}
//-------------------------------------------------------------------------
void Dsffile::sputsh(ofstream& outd, short os)
{
	union{
		short outs;
		char cs[2];
	};
	outs = os;
	outd.put(cs[0]);
	outd.put(cs[1]);
}
//-------------------------------------------------------------------------
void Dsffile::sputsh(fstream& outd, short os)
{
	union{
		short outs;
		char cs[2];
	};
	outs = os;
	outd.put(cs[0]);
	outd.put(cs[1]);
}
//-------------------------------------------------------------------------

int Dsffile::CheckDSF(String idsf)
{
	//check the magic No. to see if it is compressed
	char magic[2] = {};
	ifstream ind(idsf.c_str(), ios::binary);
	if(!ind){
		Application->MessageBox(_D("Failed to open DSF file"),
				Form1->VCmsg.c_str(), MB_OK);
		return 2;
	}
	//file is open for reading
	magic[0] = ind.get();
	magic[1] = ind.get();
	ind.close();
	if(!strncmp(magic, "7z", 2))
		return 1;
	//must not be 7z so return false (0)
	return 0;
}
//--------------------------------------------------------------------------
void Dsffile::ExtractNearDem()
{
	//come here after copying / uncompressing


}
//--------------------------------------------------------------------------
void Dsffile::PatchDem()
{

}
//---------------------------------------------------------------------------
void Dsffile::WriteDSF()
{

}
//---------------------------------------------------------------------------
int Dsffile::CopyDSF(String id, String od)
{
    bool copied = false;

	//check if already there and ask if want to overwrite
	if(FileExists(od)){
		int rv = Application->MessageBox(L"dsf file already exists in input folder.\n"
								"Do you want to overwrite it?",
							   Form1->VCmsg.c_str(), MB_YESNO );
		if(rv == 6){ //yes
			copied = CopyFileW(id.c_str(), od.c_str(),False);
				if(copied){
					return 1;
				}else{
					return 3;
				 }
		}else{
			 return 2;
		}
	}else{ //DOESN'T EXIST
		copied = CopyFileW(id.c_str(), od.c_str(),False);
		if(copied){
			return 1;
		}else{
			return 3;
		}
	}

}
//---------------------------------------------------------------------------
void Dsffile::UnzipDSF(String zdsf, String odsf)
{
	// try the shellExec
	//both archive and file to stay in the input folder
	char czdsf[MAXFILE];
	char codsf[MAXFILE];
	char pzdsf[MAXPATH];
	wchar_t wczdsf[MAXFILE];
	wchar_t wcodsf[MAXFILE];
	wchar_t wzpath[MAXPATH];
	String zpath = Form1->InDir + "\\";
	UnicodeToUtf8(czdsf, zdsf.c_str(), MAXFILE);
	UnicodeToUtf8(codsf, odsf.c_str(), MAXFILE);
	UnicodeToUtf8(pzdsf, zpath.c_str(), MAXPATH);
	wcscpy(wczdsf, zdsf.c_str());
	wcscpy(wcodsf, odsf.c_str());
	wcscpy(wzpath, zpath.c_str());
	/*
	ShellExecute(NULL, "open", "7z.exe",
 "e -o wzpath wcodsf",
  NULL,
  SW_SHOWNORMAL
  );
  */
  //using wchar
	/*
	ShellExecuteW(NULL, "open", "7z.exe",
 "e -o wczdsf, wcodsf",
  NULL,
  SW_SHOWNORMAL
  );
	  */
  int dummy = 25;
}
//---------------------------------------------------------------------------
void Dsffile::Dsf_main()
{
	//entry point
	Form1->OpenDialog1->Title = _D("Select DSF file to process");
	Form1->OpenDialog1->Filter = _D("dsf files (*.dsf) | *.DSF");
	if(Form1->OpenDialog1->Execute()){
		Indsf = Form1->OpenDialog1->FileName;
		}else{
		return;
	}

	char cdsfname[12];
	char *endptr;
	String dsfname = ExtractFileName(Indsf);
	UnicodeToUtf8(cdsfname, dsfname.c_str(), 12);
	//decode the lat/lon of the dsf coverage
	char ns[4];
	char ew[5];
	int ewp = 0;
	strncpy(ns, cdsfname, 3);
	ns[3] = 0x0;
	for(int i = 3; i < 7; i++){
		ew[ewp] = cdsfname[i];
		ewp++;
	}
	ew[ewp] = 0x0;
	double slat = strtod(ns, &endptr);
	double nlat = slat + 1;
	double wlon = strtod(ew, &endptr);
	double elon = wlon + 1;

	Outdsf = Form1->InDir + "\\" + dsfname;
	String z7name = _D("7z") + dsfname;
	z7dsf = Form1->InDir + "\\7z" + dsfname;

	//for checking
	//String odname = Outdsf;
	//Form1->log.writelog(Outdsf.c_str());//OK
	int copyok = CopyDSF(Indsf, Outdsf);
	//show copy status
		switch (copyok){
	case 1:     //copied OK
				dsmsg = _D("dsf file copied to project folder");
				Form1->log.writelog(dsmsg.c_str());
				break;
	case 2:     //overwrite declined
				dsmsg = _D("dsf file not copied."); //but still there
				Form1->log.writelog(dsmsg.c_str());
				break;
	case 3:     //copy failed so no point in continuing
				dsmsg = _D("Failed to copy dsf file.");
				Form1->log.writelog(dsmsg.c_str());
				return;
	default:
				break;
	}

	//we have copied the file or kept the existing one

	//check if compressed
	int is7z = CheckDSF(Outdsf);
			switch (is7z){
	case 0:     //not compressed
				dsmsg = _D("dsf file is uncompressed");
				Form1->log.writelog(dsmsg.c_str());
				break;
	case 1:     //compressed
                RenameFile(Outdsf, z7dsf);
				Application->MessageBox(_D("The dsf file is compressed.\n"
					"It has been remamed with a \"7z\" prefix.\n"
					"Uncompress with 7zip first"), Form1->VCmsg.c_str(), MB_OK);
				//UnzipDSF(z7dsf, Outdsf);
				break;

	case 2:     //copy failed so no point in continuing
				dsmsg = _D("Failed to open dsf file.");
				Form1->log.writelog(dsmsg.c_str());
				return;
	default:
				break;
			}

	//now we can call the extract routine

	//for now, extracted by hand so get on with it!
	//the file will be called eg -06+149.dsf as extracted from the 7zip archive
	//so we can use the String "Outdsf" for file operations
	//check we have an uncompressed dsf file
	if(!FileExists(Outdsf.c_str())){
		Application->MessageBox(_D("Can't find DSF file"),
				 Form1->VCmsg.c_str(), MB_OK);
		return;
	}

	ifstream dsfin(Outdsf.c_str(), ios::binary);
		if(!dsfin){
			Application->MessageBox(_D("Failed to open DSF file"),
				 Form1->VCmsg.c_str(), MB_OK);
			return; //no point in continuing
		}

	//create a bil/hdr/prj & bt set so that we can view it in QGis / GM

	String Dembil = ChangeFilePath(Outdsf, Form1->OutDir);
	String Demhdr = Dembil;
	String Demprj = Dembil;
	String Dembt = Dembil;  //maybe
	Dembil = ChangeFileExt(Dembil, _D(".bil"));
	Demhdr = ChangeFileExt(Demhdr, _D(".hdr"));
	Demprj = ChangeFileExt(Demprj, _D(".prj"));
	Dembt = ChangeFileExt(Dembt, _D(".bt"));

	ofstream obil(Dembil.c_str(), ios::binary);
	if(!obil){
		Application->MessageBox(_D("Failed to create bil file"),
				 Form1->VCmsg.c_str(), MB_OK);
		dsfin.close();
		return;
	}

	ofstream ohdr(Demhdr.c_str());
	if(!ohdr){
		Application->MessageBox(_D("Failed to create hdr file"),
				 Form1->VCmsg.c_str(), MB_OK);
		dsfin.close();
		obil.close();
		return;
	}

	ofstream oprj(Demprj.c_str());
	if(!oprj){
		Application->MessageBox(_D("Failed to create prj file"),
				 Form1->VCmsg.c_str(), MB_OK);
		dsfin.close();
		obil.close();
		ohdr.close();
		return;
	}
	// all files open
	//Now start to read the dsf
	auto *dshd = new DSFheader;
	auto *dsat = new DSFatom;
	auto *demr = new DEMIrec;
	long alen = 0;
	char dembuf[2042];
	char atname[5];
	char ch;
	union{
		int ian;
		char can[4];
	};
	dsfin.read((char *) dshd, sizeof(DSFheader));
	//check magic?
	if(strncmp(dshd->magic, "XPLNEDSF", 8)){
		Application->MessageBox(_D("X-Plane signature not present."),
			Form1->VCmsg.c_str(), MB_OK);
		dsfin.close();
		obil.close();
		ohdr.close();
		oprj.close();
		return;
	}
	//now read the atoms
	for(int i = 0; i < 4; i++){
	dsfin.read((char *)dsat, sizeof(DSFatom));
	ian = SWAP_INT32(dsat->ianame);
	strncpy(atname, can, 4);
	alen = dsat->asize;
	dsfin.seekg(alen-8, ios::cur);
	}
	//now at DEMS
	dsfin.read((char *)dsat, sizeof(DSFatom));
	ian = SWAP_INT32(dsat->ianame);
	strncpy(atname, can, 4);
	alen = dsat->asize; //not interested address of eof before md5
	//read DEMI atom
	dsfin.read((char *)dsat, sizeof(DSFatom));
	//nothing to do with this info
	dsfin.read((char *)demr, sizeof(DEMIrec));
	//check stuff?
	dsfin.read((char *)dsat, sizeof(DSFatom)); //DEMD
	//store this address for re-writing?
	//can get address of FAR DEMI here.
	//now at the start of the dems
	//all the dem blocks from system dsf files will be 1201 x 1201
	//create the array
	short inpt;
	int cols = 1201, rows = 1201;
	short **demblk;
	demblk = new short* [rows];
	for(int i = 0; i < rows; i++){
		demblk[i] = new short [cols];
	}
	//read the dem block row major starting sw corner.
	for(int i = rows-1; i > 0; i--){
		for(int j = 0; j < cols; j++) {
			demblk[i][j] = sgetsh(dsfin);
		}
	}
	dsmsg = _D("Dem values read");
	Form1->log.writelog(dsmsg.c_str());
	//should have the whole block read now 0,0 is NW corner
	dsfin.close();

	//write bil file
	for(int i = 0; i < rows; i++){
		for(int j = 0; j < cols; j++){
			sputsh(obil, demblk[i][j]);
		}
	}

	obil.close();
	dsmsg = _D("BIL file written");
	Form1->log.writelog(dsmsg.c_str());
	//now write the header file
	char fileln[80];
	int nbands = 1;
	int nbits = 16;
	int dwidth = cols;
	int dheight = rows;
	double xdim = 0, ydim = 0;
	xdim = (1.0/1200);
	ydim = xdim;
	double ULYMAP = nlat;// + (ydim / 2);
	double ULXMAP = wlon;// - (xdim / 2);
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
	sprintf(fileln,"NODATA        -32768\n");
	ohdr << fileln;
	sprintf(fileln,"ULXMAP        %0.9f\n",ULXMAP);
	ohdr << fileln;
	sprintf(fileln,"ULYMAP        %0.9f\n",ULYMAP);
	ohdr << fileln;
	sprintf(fileln,"XDIM        %0.9f\n",xdim);
	ohdr << fileln;
	sprintf(fileln,"YDIM        %0.9f\n",ydim);
	ohdr << fileln;
	ohdr.close();
	/*
	dsmsg = _D("HDR file written");
	Form1->log.writelog(dsmsg.c_str());
	*/
	//prj file
	oprj << "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",";
	oprj << "SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM";
	oprj << "[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]";
	oprj.close();
	/*
	dsmsg = _D("PRJ file written");
	Form1->log.writelog(dsmsg.c_str());
	*/
	delete dsat;
	delete demr;
	delete dshd;

	//write the .bt file
	ofstream obt(Dembt.c_str(), ios::binary);
	if(!obt){
		Application->MessageBox(_D("Failed to create .bt file"),
				 Form1->VCmsg.c_str(), MB_OK);
		for(int i = 0; i < cols; i++){
			delete [] demblk[i];
		}
		delete [] demblk;
		return;
	}
	auto *bth = new btheader;
	strcpy(bth->ftype, "binterr1.3");
	bth->btwid = cols;
	bth->bthgt = rows;
	bth->dsize = 2;
	bth->fpflag = 0;
	bth->hunits = 0;
	bth->utmzone = NULL;
	bth->datum = 6326;
	bth->leftext = wlon - (xdim / 2);
	bth->rightext = elon + (xdim / 2);
	bth->botext = slat - (ydim / 2);
	bth->topext = nlat + (ydim / 2);
	bth->extproj = 0;
	bth->vscale = 1.0;
	memset(bth->padding, NULL, sizeof(bth->padding));
	obt.write((char *)bth, sizeof(btheader));
	//int k = 0;
	//write out the data
	for(int j = 0; j < cols; j++){
		for(int i = rows-1; i >= 0; i--){
			sputsh(obt, demblk[i][j]);

		}
    }

	obt.close();
	dsmsg = _D("BT file written");
	Form1->log.writelog(dsmsg.c_str());


	for(int i = 0; i < cols; i++){
		delete [] demblk[i];
	}
	delete [] demblk;

	delete bth;

}
//--------------------------------------------------------------------------

