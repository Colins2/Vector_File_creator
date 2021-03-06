//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <Filectrl.hpp>


#pragma hdrstop

#include "Vec_creator.h"
#include "logmsg.h"
#include "Runway.h"
#include "RCoords.h"
#include "VFCheaders.h"
#include "Gridwriter.h"
#include "walker.h"
#include "Dempatch.h"
#include "DSF_Utils.h"
#include "MuxpWriter.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"


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

	log.writelog(VCversion.c_str());
	log.writelog(Copyright.c_str());
	Form1->Caption = VCmsg;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Exit1Click(TObject *Sender)
{
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
	//take the txt file saved from GPSVisualizer.com and extract the elevations
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
	dsffile.Dsf_main();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PatchDSFDEM1Click(TObject *Sender)
{
	//Apply the patch grid to the dsf grid and write the dsf file
	//need to find the hashkey code?

	Dempatcher dempatcher;
	dempatcher.PatchDSF();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::PatchDSFBIL1Click(TObject *Sender)
{
	//apply the patch file to the generated BIL file for testing / checking
	Dempatcher dempatcher;
	dempatcher.PatchBIL();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::CreateCSVfilefromKMLprofile1Click(TObject *Sender)
{
	//create a csv file from the KML runway profile
	rway.KMLprofile2CSV();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Createmuxpinputfile1Click(TObject *Sender)
{
	//write a muxp file
	MuxpWriter muxpwriter;
	muxpwriter.MuxpMain();

}
//---------------------------------------------------------------------------

void __fastcall TForm1::About2Click(TObject *Sender)
{
	//make an about dialog box with simple details
}
//---------------------------------------------------------------------------

