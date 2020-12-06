//---------------------------------------------------------------------------

#ifndef walkerH
#define walkerH
#include <string.h>
#include <vector>

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
	void Split_csv(char *s, vector<string> &vcsv, int numstr);




};





//---------------------------------------------------------------------------
#endif
