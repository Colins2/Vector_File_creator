//---------------------------------------------------------------------------
#include <stdio.h>
#include <fstream> //maybe use?
#include <string.h>
#include <vector>

#pragma hdrstop
#include "shape.h"
#include "vecstore.h"
#include "Vec_creator.h"

using namespace std;

/*
We probably don't need this class to be defined like this but it is good practise
and will make the store available from other places? - maybe?
*/


//---------------------------------------------------------------------------
#pragma package(smart_init)
Vecstore::Vecstore()
{
	//don't think we need this - what is smart_init?
	//vector<LatLonPoint> vs;
	Form1->log.smsg = _D("Vecstore object created");
	Form1->log.writelog(Form1->log.smsg.c_str());
}
//--------------------------------------------------------------------------
Vecstore::~Vecstore()
{
	Form1->log.smsg = _D("Vecstore object destroyed");
	Form1->log.writelog(Form1->log.smsg.c_str());
}
//--------------------------------------------------------------------------
/*
void Vecstore::OpenVecStore()
{
	vector<LatLonPoint> vs;
}
 */
//-------------------------------------------------------------------------

