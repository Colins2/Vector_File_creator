//---------------------------------------------------------------------------

#ifndef RunwayH
#define RunwayH
#include <System.Classes.hpp>
#include <string.h>
#include <vector>
#include <math.h>
#include "RCoords.h"
#include "VFCheaders.h"

using namespace std;



//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
class Runway
/*
It is intended that the input will either be in the boxex on the input form
or be read from a project apt.dat. This way, you can select any project and
get the runway polygon from there.
*/
{
private:

public:
	Runway();
	~Runway();
	/*
	struct LatLonPoint //maybe use
	{
		double lat;
		double lon;
	};
    */
	//given input coordinates - from anywhere

	double lolat;
	double lolon;
	double hilat;
	double hilon;
	double mhdg; //low end to high end i.e. always 0 to 179
	double width;
	double chdg; //calculate it anyway
	//String AptName;
	//String icao;
	//no need for the length
	double RAD = (180/M_PI);
	void setlolat(double z){lolat = z;};
	//void setaname(String s){AptName = s;};

	//functions can use local variables, no need to be in here
	String rmsg; // = L"In Runway Class";
    wchar_t rwmsg[80];
	String Infile;
	String Shapefile;
	String Shxfile;
	String DBFfile;
	String Projfile; //shapefile projection file
	//a main function for this class
	void GetAptDatCoords(); //open the file and read the coordinates
	void GetDirectInput(); //using a form
	void RwyMain(int source); //entry point to this source file
	void SplitLine(char *s, vector<string> &split, int numstr);
	int LL_Mantissa_check(vector<string> &split, int fn);
    void KMLprofile2CSV();




};




//---------------------------------------------------------------------------
#endif
