//shapefile structs
#ifndef _SHAPE_H
#define _SHAPE_H


#pragma pack (push, 1)
//File header
typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct{
char fil_code[4];  //9994 big endian     (0x0000270a)
int unu1;  //**********************
int unu2;  //                     *
int unu3;  //  Big endian, unused *
int unu4;  //                     *
int unu5;  //**********************
uint fil_len;  //Big endian in 16 byte words
uint fil_ver; //1000 little endian   (0x03e8)
int shp_typ; // Little endian *         //only interested in  polygons for now
double xmin; // Little endian doubles  *
double ymin; //         "              *
double xmax; // 		"			   *
double ymax; //         "              *
double zmin; //   unused               *     value 0
double zmax; //      "                 *        "
double mmin; //      "                 *        "
double mmax; //      "                 *        "
}shp_header;

/*
  Value Shape Type
0	Null Shape
1   Point
3	PolyLine
5	Polygon
8 	MultiPoint
11 	PointZ
13 	PolyLineZ
15 	PolygonZ
18 	MultiPointZ
21 	PointM
23 	PolyLineM
25 	PolygonM
28 	MultiPointM
31 	MultiPatch
*/

//record header
typedef struct{
int recno; //Big endian int
int content;//   "         content length
}shp_rec_header;
/*
Record numbers begin at 1.
The content length for a record is the length of the record contents section measured in
16-bit words. Each record, therefore, contributes (4 + content length) 16-bit words
toward the total length of the file, as stored at Byte 24 in the file header.

Points start at byte 44 + 4 * numparts (so normally byte +48 for XPlane use)
content is starting point of the points as above + (num points * 8 * 2)
giving the number of bytes. Divide by 2 to give the number of 16bit words and
then byteswap it to give a big endian number(if required).
*/

//shx file record
typedef struct{
int shp_offset; //Big endian
int content;    //    "
}shx_rec;

//point record
typedef struct{
int shp_typ;
double xpt;
double ypt;
}pt_rec;

//multipoint record (1)
typedef struct{
int shp_typ;
double bxmin;
double bymin;
double bxmax;
double bymax;
int numpts;
}mpt_rec;

//multipoint record (2)
// array of type points

//polyline record
typedef struct{
int shp_typ;
double bxmin;
double bymin;
double bxmax;
double bymax;
int numparts;
int numpoints;
int parts; //index to first point in parts - not relevant with single rings
}shp_poly_rec; //for writing for now, only ever 1 ring

typedef struct{
int shp_typ;
double bxmin;
double bymin;
double bxmax;
double bymax;
int numparts;
int numpoints;
//int parts;
}polyrr; //used for reading maybe more than 1 part

//polyline arrays written as long (x) - lat (y)! i.e. same as UTM format
typedef struct{
double xpt;
double ypt;
}polyb;


//dbf header
typedef struct{
char dbhstart; //0x03
uchar year;
uchar month;
uchar day;
uint reccount; //number of records in the table
short head_bytes; //number of bytes in header
short data_width;   //number of bytes in a record
char fill[20]; //3 fields, 3, 13 & 4 all reserved
}dbf_header;

//dbf field descriptor 32 bytes
typedef struct{
char field_name[11];
char field_type;
int fill;  //field address in memory - not used for disk file
uchar field_len;
uchar field_decpl; //decimal count
short int res; //reserved for later versions
char waid; //work area ID, not used
short int res2; //reserved for later versions
char set_fields_flag; // not sure if used?
char reserv[8];
}dbf_field_desc;

struct dbfxprec{
	char del_mark;
	char name[20];	//AptName
	//char apt_type[6]; //land, sea, heli
	char feature[12]; //runway, boundary etc.
	//char name_id[8]; //if we can,
}; //32 bytes

typedef struct{
char dbrstart; //0x20
char lineno[4];
char type[4];
char desc[40];
}dbfr; //just for GTD to shapefile

typedef struct{
char dbrstart; //0x20
char entno[4];
char nodecnt[4];
char type[4];//holds the colour which will be the type
char desc[20];//don't need more(yet)
}dbfsr; //record for shapefiles from a dxf file
/*
typedef struct{
int layno;
int fcol;
int ftype;
char shpname[256];
char feature[40];
}aeprec;
*/

//#pragma pack (pop)
#endif

