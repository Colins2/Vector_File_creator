//---------------------------------------------------------------------------

#ifndef RCoordsH
#define RCoordsH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
//---------------------------------------------------------------------------
class TRwyInput : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label1;
	TMemo *Memo1;
	TLabel *Label2;
	TEdit *LElat;
	TLabel *Label3;
	TEdit *LElon;
	TEdit *HElat;
	TEdit *HElon;
	TLabel *Label4;
	TLabel *Label5;
	TComboBox *HElatns;
	TComboBox *LElatns;
	TComboBox *LElonew;
	TComboBox *HElonew;
	TButton *SubmitBtn;
	TLabel *Label8;
	TEdit *Rwidth;
	TEdit *AptName;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label9;
	TEdit *icao;
	TLabel *Label10;
	TEdit *Tilename;
	void __fastcall SubmitBtnClick(TObject *Sender);
private:	// User declarations
public:		// User declarations

	__fastcall TRwyInput(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRwyInput *RwyInput;
//---------------------------------------------------------------------------
#endif
