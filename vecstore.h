//---------------------------------------------------------------------------

#ifndef vecstoreH
#define vecstoreH
#include <vector>
#include "shape.h"
//---------------------------------------------------------------------------
using namespace std;

class Vecstore
{
	private:
	public:
	Vecstore();
	~Vecstore();

struct VLatLonPoint
{
	double lat;
	double lon;
};
 	int test = 0;
//void OpenVecStore();
VLatLonPoint Vllp;
vector<VLatLonPoint> vs;
vector<VLatLonPoint>::iterator vsi = vs.begin();
//VLatLonPoint vllp;
//void PushBack(LatLonPoint *sllp){vs.push_back(*sllp);};

//shouldn't need any functions or variables as it is only a store
//accessible from other routines.

};




#endif
