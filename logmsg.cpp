//---------------------------------------------------------------------------



#pragma hdrstop
#include "logmsg.h"
#include "Vec_creator.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)
//---------------------------------------------------------------------------
LogMsg::LogMsg()
{
	//initialize
}
//--------------------------------------------------------------------------
LogMsg::~LogMsg()
{
	//destroy
}
//---------------------------------------------------------------------------
void LogMsg::writelog(wchar_t *m)
{
	Form1->Memo1->Lines->Add(m);

}
//---------------------------------------------------------------------------