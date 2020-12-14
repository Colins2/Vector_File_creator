//---------------------------------------------------------------------------

#ifndef VecwriterH
#define VecwriterH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <string.h>
#include <vector>

#include "Vec_creator.h"
#include "VFCheaders.h"

class Vecwriter
{
	private:

	public:
	Vecwriter();
	~Vecwriter();

	String vmsg;
	String Vecname;
	String Vecicao;
	String Vecfeature;
	String filestem;
    String Vectile;
	//********************************************
	//use these for constructing a grid shapefile.
	double nbound = 0;
	double sbound = 0;
	double wbound = 0;
	double ebound = 0;
	//*******************************************
	double rwidth = 0;
	void SetVecname(String s){Vecname = s;};
	void SetVecfeature(String s){Vecfeature = s;};
	void SetVecfilestem(String fs){filestem = fs;};
	void SetVecicao(String ic){Vecicao = ic;};
	void WriteVectorFile(vector<LatLonPoint> vllp, int numpts);
	void SetRunwayWidth(double w){rwidth = w;};
	double Getrwidth(){return rwidth;};
	double Getnbound(){return nbound;};
	double Getsbound(){return sbound;};
	double Getebound(){return ebound;};
	double Getwbound(){return wbound;};
};






#endif
