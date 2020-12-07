//---------------------------------------------------------------------------

#ifndef DSF_UtilsH
#define DSF_UtilsH
#include "Vec_creator.h"

//using namespace std;

class Dsffile
{
private:

public:
	Dsffile();
	~Dsffile();

	//members
	String dsmsg;
	String Indsf;
	String Outdsf;
	String z7dsf;

	//methods
	void ExtractNearDem();
	void ExtractFarDem();
	void PatchDem();
	void WriteDSF();
	int CopyDSF(String infn, String outfn);
	void UnzipDSF(String zipdsf, String odsf);
	void Dsf_main();
	int CheckDSF(String idsf);
	static short sgetsh(ifstream& ind);
	static void sputsh(ofstream& outd, short os);
    static void sputsh(fstream& outd, short os);



};






//---------------------------------------------------------------------------
#endif
