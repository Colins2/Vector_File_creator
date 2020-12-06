//---------------------------------------------------------------------------

#ifndef DempatchH
#define DempatchH
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class Dempatcher
{
	public:
	Dempatcher();
	~Dempatcher();

	String dmsg;
	void PatchHgt();
	void fputsh(FILE *of, short sh);
	short fgetsh(FILE *inf);
	short fgetles(FILE *inf);
	void fputles(FILE *of, short sh);
	void PatchDSF();
    void PatchBIL(); //for testing purposes
};


//---------------------------------------------------------------------------
#endif
