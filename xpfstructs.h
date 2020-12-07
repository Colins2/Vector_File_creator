#ifndef xpfstructsH
#define xpfstructsH

//----------------------------------------------------------------------
//put the file strucftures in here
//DEMI field of the .dsf file
//typedef uint8
#pragma pack (push, 1)
struct DEMIrec{
	unsigned char dver;
	unsigned char dbpp;
	unsigned short dflags;
	int dwidth;
	int dheight;
	float dscale;
	float doffset;
};

//----------------------------------------------------------------------
struct DSFheader{
	char magic[8];
	int version;
};
//---------------------------------------------------------------------
struct DSFatom{
	int ianame;
	long asize; //includes 8 bytes for this struct
};
//---------------------------------------------------------------------
//struct for a header in my .hgt file

struct HGThdr{
	char Title[30];
	char layer; //1 for normal, 0 for (distance)
	unsigned char dembpp;
	unsigned short demflags;
	unsigned int demwidth;  //number of entries
	unsigned int demheight;
	float demscale;
	float demoffset;
};

//----------------------------------------------------------------------
//struct for a project file

struct prjhdr{
	char title[24];
	char description[40];
	int atoms;
};

//----------------------------------------------------------------------

struct prjrec{
	char atom[4];
	int alength;
	long dataddr;
};

//----------------------------------------------------------------------

struct prjdets{
	//put in here the details from the PROP atom for display purposes
	int nbound;
	int sbound;
	int wbound;
	int ebound;
	char planet[10];
	char creator[30];
};

//----------------------------------------------------------------------

struct prxtail{
	//store the addresses of the dems
	unsigned long DEMSaddr;
	unsigned long BMdemaddr;
	int lenBMdata;
	unsigned long FMdemaddr;
	int lenFMdata;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//List of major & sub atoms
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
HEAD          Atom of Atoms, only 1
	PROP
DEFN contains 4 sub atoms
	TERT
	OBJT
	POLY
	NETW
DEMN
	GEOD may contain sub atoms POOL and SCAL
	POOL 16 bit
	SCAL 16
		  "
		  "
	PO32 32 bit
	SC32 32
DEMS DEM header
	DEMI   DEM info
	DEMD   DEM data
CMDS - Commands
*/
//------------------------------------------------------------------------
// BT File (Binary Terrain used by VTP and others)


struct btheader{
	char ftype[10];   //binterr1.3
	int btwid; //Columns east-west
	int bthgt; //Rows north-south
	unsigned short int dsize; //Bytes per elevation grid 2 or 4
	short int fpflag; //1 if floating point otherwise signed integers
	short int hunits; //horizontal units, 0 = deg, 1 = metres, 2 = feet, 3 = us survey feet
	short int utmzone; //indicates UTM zone if in metres
	short int datum; //will be 6326 for WGS84
	double leftext;
	double rightext;
	double botext;
	double topext;
	short int extproj; //0 projection in this header, 1 in external .prj file
	float vscale; //vertical scale of units, usually 1.0
	char padding[190];
};

//-------------------------------------------------------------------------
//BMP header file

struct bmpheader{
	short bm;      //value    19778 dec
	int filelen;   // to calculate (horiz*3)+pad if reqd * vert + 54
	int z1;
	int data_st;   //54 as there won't be any colour chart
	int forty;     //40
	int im_wid;    //in pixels so 1201 x 1201 for example
	int im_ht;     //as wid
	short one;     //1
	short col_depth;  //have to count this as we go (if required?)
	int z2;
	int data_len;  //filelen - 54
	int z3;
	int z4;
	int z5;
	int z6;
	};
#pragma pack (pop)
//-------------------------------------------------------------------------


#endif
