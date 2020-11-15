//---------------------------------------------------------------------------

#ifndef Vec_creatorH
#define Vec_creatorH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.Menus.hpp>

#include "logmsg.h"
#include "Runway.h"
#include "RCoords.h"
#include "vecstore.h"
#include "Apt_area.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMemo *Memo1;
	TMainMenu *MainMenu1;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TMenuItem *File1;
	TMenuItem *Vector1;
	TMenuItem *Openinputfile1;
	TMenuItem *Exit1;
	TMenuItem *CreateShapefileset1;
	TMenuItem *CreateMapInfofile1;
	TMenuItem *CreateDXFfile1;
	TMenuItem *Runway1;
	TMenuItem *MakeRunwayPolygon1;
	TMenuItem *OpenVectordatafile1;
	TMenuItem *RunwayPolyFromFile;
	TMenuItem *RunwayPolyFromAptDat1;
	TMenuItem *Area1;
	TMenuItem *CreateAreaPolyFromFile1;
	TMenuItem *CreateAreaPolyFromKMLfile1;
	TMenuItem *Creategridforairportarea1;
	TMenuItem *Extractdatafromwalktextfile1;
	TMenuItem *Creategridwalkfile1;
	TMenuItem *DEMS1;
	TMenuItem *PatchtileDEM1;
	void __fastcall Exit1Click(TObject *Sender);
	void __fastcall Openinputfile1Click(TObject *Sender);
	void __fastcall CreateShapefileset1Click(TObject *Sender);
	void __fastcall CreateMapInfofile1Click(TObject *Sender);
	void __fastcall CreateDXFfile1Click(TObject *Sender);
	void __fastcall MakeRunwayPolygon1Click(TObject *Sender);
	void __fastcall RunwayPolyFromAptDat1Click(TObject *Sender);
	void __fastcall CreateAreaPolyFromKMLfile1Click(TObject *Sender);
	void __fastcall Creategridforairportarea1Click(TObject *Sender);
	void __fastcall Extractdatafromwalktextfile1Click(TObject *Sender);
	void __fastcall Creategridwalkfile1Click(TObject *Sender);
	void __fastcall PatchtileDEM1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	LogMsg log;
	Runway rway;
	Aptarea aptarea;
	//Vecstore vecstore;
	String Copyright = L"© Colin Sare-Soar 2020";
	String VCversion = L"Vector File Creator v1.0.0.5";
	String VCmsg = L"Vector File Creator";
	String CurrDir;
	String OutDir;
	String InDir; //for holding text files, if ever needed






	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
