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
	void PatchDem();
	void fputsh(FILE *of, short sh);
	short fgetsh(FILE *inf);
	short fgetles(FILE *inf);
	void fputles(FILE *of, short sh);
};


//---------------------------------------------------------------------------
#endif
