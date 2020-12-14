//---------------------------------------------------------------------------

#ifndef GridwriterH
#define GridwriterH
#include "VFCheaders.h"
#include "Vec_creator.h"
#include "shape.h"
//---------------------------------------------------------------------------

class Gridwriter
{
	private:

	public:
	Gridwriter();
	~Gridwriter();


	String gmsg;
	double gridspace; //in degrees

	String filestem;
	String Vecname;

	void Writegrid();
	void Setfilestem(String s){filestem = s;};
	void SetVecname(String v){Vecname = v;};
    void ExtractAptName(char *s);




};



#endif
