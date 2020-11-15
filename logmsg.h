//---------------------------------------------------------------------------

#ifndef logmsgH
#define logmsgH
#include <System.Classes.hpp>
#include <string.h>
//make a class just for logging


class LogMsg
{
private:
public:
LogMsg();
~LogMsg();

wchar_t wmsg[128];
String smsg;

void writelog(wchar_t *s);

};






//---------------------------------------------------------------------------
#endif
