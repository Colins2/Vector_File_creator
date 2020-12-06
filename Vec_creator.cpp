//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> //dunno if we need it in this file
#include <vector>
#include <Filectrl.hpp>


#pragma hdrstop

#include "Vec_creator.h"
//#include "shape.h" //for sructures etc.
#include "logmsg.h"
#include "Runway.h"
#include "RCoords.h"
//#include "vecstore.h"
#include "VFCheaders.h"
#include "Gridwriter.h"
#include "walker.h"
#include "Dempatch.h"
#include "DSF_Utils.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

/*
The idea of this utility is to create a vector file or set.
Initially it will be a shapefile set with maybe MapInfo /KML to follow
Input will be initially a csv containing just pairs of coordinates.
Later we will extract the coordinates from a KML file.


*/

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	Memo1->Lines->Clear();
	CurrDir = GetCurrentDir();
	OutDir = CurrDir + L"\\Output";
	InDir = CurrDir + L"\\Input";
	if(!DirectoryExists(OutDir))
		CreateDir(OutDir);
	if(!DirectoryExists(InDir))
		CreateDir(InDir);
	//LogMsg log;
	//Runway rway;
	//Vecstore vecstore;
	log.writelog(VCversion.c_str());
	log.writelog(Copyright.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
    //vecstore.vs.clear();
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Openinputfile1Click(TObject *Sender)
{
	//for now we'll just open a text file (csv)

}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateShapefileset1Click(TObject *Sender)
{
	//create a shapefile set - only interested in polygons for now

}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateMapInfofile1Click(TObject *Sender)
{
	//maybe in the future - not a priority
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateDXFfile1Click(TObject *Sender)
{
	//Definitely NOT a priority!
}
//---------------------------------------------------------------------------
void __fastcall TForm1::MakeRunwayPolygon1Click(TObject *Sender)
{
	//input runway end coordinates by hand plus direction measured from GE
	//calculate the 4 corner points of the runway and export as a vector file
	//see if we are in the class
	//log.writelog(rway.rmsg.c_str()); //yay!
	rway.RwyMain(1);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::RunwayPolyFromAptDat1Click(TObject *Sender)
{
	//read a local apt.dat file and get the runway coordinates
	rway.RwyMain(2);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::CreateAreaPolyFromKMLfile1Click(TObject *Sender)
{
	aptarea.Aamain();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Creategridforairportarea1Click(TObject *Sender)
{
	Gridwriter gridwriter;
    gridwriter.Writegrid();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Extractdatafromwalktextfile1Click(TObject *Sender)
{
	//take the txt file saved from GPSVisualizer.com and extract tha elevations
	Walker walker;
    walker.ImportWalkerDem();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Creategridwalkfile1Click(TObject *Sender)
{
	//create a "walk or GPS" file for inputting to GPS Visualizer
	Walker walker;
    walker.CreateWalkerTextFile();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::PatchtileHGT1Click(TObject *Sender)
{
	//Load the data from the edited / downloaded patch and merge it into
	//an existing DEM / HGT file

	Dempatcher dempatcher;
	dempatcher.PatchHgt();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::ExtractDSFDEM1Click(TObject *Sender)
{
	//extract the DEM block from a DSF file
	Dsffile dsffile;
	//vmsg = _D("Ready to extract....");
	//log.writelog(vmsg.c_str());
	dsffile.Dsf_main();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PatchDSFDEM1Click(TObject *Sender)
{
	//Apply the patch grid to the dsf grid and write the dsf file
	//need to find the hashkey code.
	//Dsffile dsffile;
	//vmsg = _D("Ready to patch....");
	//log.writelog(vmsg.c_str());
	Dempatcher dempatcher;
	dempatcher.PatchDSF();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PatchDSFBIL1Click(TObject *Sender)
{
	//apply the patch file to the generated BIL file for testing / checking
	//Dsffile::Dsffile;
	Dempatcher dempatcher;
	dempatcher.PatchBIL();
}
//---------------------------------------------------------------------------

