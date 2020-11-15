//---------------------------------------------------------------------------

#ifndef walkerH
#define walkerH
#include <string.h>

#include "Vec_creator.h"


using namespace std;

class Walker
{
private:

public:
	Walker();
	~Walker();

	String wmsg;
	void CreateWalkerTextFile();
	void ImportWalkerDem();
    void PutShort(ofstream out, short s);





};





//---------------------------------------------------------------------------
#endif
