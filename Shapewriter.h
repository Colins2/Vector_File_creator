//---------------------------------------------------------------------------

#ifndef ShapewriterH
#define ShapewriterH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "VFCheaders.h"
#include "Vec_creator.h"
#include <vector>
#include <string.h>

using namespace std;

class Shpwriter
{
private:

public:
Shpwriter();
~Shpwriter();

//put some variables that can be populated by other classes
String Shpname;
String Shpfeature;
String filestem;
String projfile; //standatd projection info
String shmsg;
double nbound;  //for the shapefile itself
double sbound;  //          "
double wbound;  //          "
double ebound;   //          "
int dbffields;


void SetShpname(String s){Shpname = s;};
void SetShpfeature(String s){Shpfeature = s;};
void SetShpfilestem(String fs){filestem = fs;};
void Writeshapefile(vector<LatLonPoint> llp, int numpts);
void vstrnset(char *buff, char ch, int count);
void ShpMain();

};



















#endif
