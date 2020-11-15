//---------------------------------------------------------------------------

#ifndef ShapewriteH
#define ShapewriteH
//---------------------------------------------------------------------------



class Shpwriter:
{
private:

public:
Shpwriter();
~Shpwriter();

//put some variables that can be populated by other classes
String Shpname;
String Shpfeature;


void SetShpname(String *s){Shpname = s;};
void SetShpfeature(String *s){Shpfeature = s;};



};



















#endif
