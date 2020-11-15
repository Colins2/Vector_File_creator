#ifndef VFCheadersH
#define VFCheadersH
//--------------------------------------------------------------------------

#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))
#define SWAP_INT16(x) (((x >> 8) & 255) + (x << 8 ))


template <typename T1, typename T2>
double diff(const T1& lhs, const T2& rhs)
{
  return lhs - rhs;
}


struct LatLonPoint //maybe use
	{
		double lat;
		double lon;
	};

struct LatLonElevPt    //so we can write the imported values to a file
	{
		double lat;
		double lon;
		short int elev;
	};  //can use the existing grd structures



//used for all polygons in my utilities.
//Runway polys will be calculated from the runway end
//positions + width + true heading(calculated) + blast pad / run off
//airport boundaries are polys anyway; maybe we can get aprons etc.
//no need for taxiways

//In this application we only need to store 1 record, either a runway polygon
//or an airport area polygon. It keeps the points for further use - maybe?

// change the names of the structures to prevent confusion

//struct tile_vector_header{
struct VFC_vector_file_header{
	char tvf_magic[16];
	int numrecs;
	char pad[12];
	};  //32 bytes

//struct tile_vector_polygon_header{
struct VFC_polygon_header{
	int recno;
	int numpts;
	//**********************************************
	//bounding box for use in creating shapefile grid, or mapinfo tab?
	double nbound;
	double sbound;
	double wbound;
	double ebound;
	//**********************************************
	int s_type; //because shapefiles have it - maybe for the future?
	char v_icao[8];
	char v_apt_type[6];
	char v_feature[9];
	char v_name_ID[8]; // maybe need a name or other ID?
	char pad[117]; //future proofing :-)
	}; //now 192 butes

//struct tile_poly_point{ //maybe can use LatLonPoint? or confusion?
struct VFC_poly_point{
	double plat;
	double plon;
	}; //16 bytes
//--------------------------------------------------------------------------
struct Grd_header{
	char grd_magic[16];
	int recnum;
};
//--------------------------------------------------------------------------
struct Grd_data{
	//pixel positions for writing the grid in DEM format
	int eastpix;
	int westpix;
	int northpix;
	int southpix;
	int xrange;
	int yrange;
};
//no need for a struct to write single values, only storing elevations??
//--------------------------------------------------------------------------
//sentences for the .prj files cannot define them
/*
char prj1[] = {"GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\","};
char prj2[] = {"SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM"};
char prj3[] = {"[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]"};
 */

//--------------------------------------------------------------------------
#endif
