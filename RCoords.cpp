//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <stdlib>
#include <math.h>
#include <stdio.h>

#include "RCoords.h"
#include "Vec_creator.h"
#include "Runway.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRwyInput *RwyInput;
//---------------------------------------------------------------------------
__fastcall TRwyInput::TRwyInput(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TRwyInput::SubmitBtnClick(TObject *Sender)
{
	//transfer the values to the class structure and close
	wchar_t *endptr;
	String temp;
	double tempd;
	wchar_t inval[20];
	//UnicodeToUtf8(LElat->Text, inval);
	Form1->rway.lolat = LElat->Text.ToDouble();
	tempd = LElat->Text.ToDouble();
	//tempd = wcstod(temp, &endptr);
	Close();
}
//---------------------------------------------------------------------------

