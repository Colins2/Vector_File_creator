//---------------------------------------------------------------------------

#ifndef Apt_areaH
#define Apt_areaH

#include <System.Classes.hpp>
#include <string.h>
#include <vector>
#include <math.h>
#include "VFCheaders.h"

//---------------------------------------------------------------------------

using namespace std;

class Aptarea
{
private:

public:

	Aptarea();
	~Aptarea();

	String amsg;
	String Inkml;
	String Shapefile;
	String Shxfile;
	String DBFfile;
	String Projfile;
	String AptName;
	String Atxtfile;
    String GridFile;

	void setaname(String s){AptName = s;};
	void Aamain();
	void ltrim(char *s);



};

#endif
