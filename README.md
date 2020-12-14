README.md file for Vector File Creator.
This is an Embarcadero C++ project using RAD Studio 10.3 (Free community edition)

This project is meant to be used with X-Plane 11 scenery. It could be adapted for more general use but there are plenty of other 
utilities available that will do what VFC does. VFC is quite specialised in it's application, as follows:

Vector File Creator:
This started life as a way to get runway information in the form of a polygon from the apt.dat file(s) for display purposes.
It was always the intention to display this polygon over a raster of the DEM data from various sources with a view to changing 
the data to more correctly model the 'real world' - basically, that which is seen on Google Earth.

After the runway endpoints are read, either from an apt.dat file or from coordinates read in from the dialog box, the corner 
points are calculated and the resulting polygon created as a shapefile set.
After some experimentation, I found that  this was too small an area to be working with due to the size of the pixels. 1 pixel
is approximately a 30 metre square in X-Plane. Although more detailed elevation data is available, X-Plane cannot display it,
therefore each tile is 1201 x 1201 pixels in size, equating to SRTM 3 arcsec data(from which I believe X-Plane terrain is modelled).

I have added the function to create an airport area polygon from a Google Earth KML file. This is also saved as a shapefile set.
 
VFC creates a grid of 1 x 1 pixel squares covering the airport area and then creates a 'walk' file, tabulating the centres 
of each pixel in a csv file. The grid is saved as a shapefile set, while the 'walk' is saved as a csv file.

All of the above can be viewed layered in a GIS or in Google Earth. It is possible to get the data for these points with various
utilities. GPSvisualizer.com is the site I have used for testing.

All this data can be compared onscreen and edited by various means. Using QGis and the Serval plugin, for example allows
direct editing of the SRTM data, or you can edit/create a patch file and then apply the patch. Note that although you can patch
or edit a SRTM DEM file it will need to be processed with Meshtool, which is a simple operation, but might affect other 
existing airports.
VFC also has the ability to patch the dsf dem data directly, for immediate use. The TIN is not affected, just the underlying 
elevation data.

For some airports, this will be sufficient. For others you may need to do other processing using muxp or Meshtool.

VFC will create a .muxp file provided the data has been extracted. It requires the airport_area file, the runway file and a runway
profile file. It will not read / use any other files such as for scenery objects, draped polygons etc.

VFC is intended to be used either stand-alone or with muxp to modify the underlying mesh before creating scenery.

See the user manual for more details.
