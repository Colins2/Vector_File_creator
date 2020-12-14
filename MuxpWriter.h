//---------------------------------------------------------------------------
/*
	We'll try an experiment for 'cleaner code' and populate the class with some
	of the values that we need first. The coordinates will have to be read into
	a vector?
*/
#ifndef MuxpWriterH
#define MuxpWriterH

#include <System.Classes.hpp>
#include <vector>
#include "VFCheaders.h"
using namespace std;
//---------------------------------------------------------------------------
struct rwaydata{
	/*
	double rlat;
	double rlon;
	double relev;
	*/
	char rlat[16];
	char rlon[16];
	char relev[8];
};

class MuxpWriter
{
	private:

	public:
	MuxpWriter();
	~MuxpWriter();

	double rwidth;
	double nbound;
	double sbound;
	double wbound;
	double ebound;
	vector<VFC_poly_point> Abound;
	vector<rwaydata> rdat;
    String fmuxp; //filename
	String mxmsg;
	String Aicao;
	string Atile;
    string Apname;
	void WriteMuxp();
	void ReadAptArea();
	void ReadRunwayProfile();
	void MuxpMain();



};








#endif
