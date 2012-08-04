#include "eHimage.h"
#include "eHutils.h"
#include "eHfacemodel.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <iostream>
#include <fstream>

#include <string.h>
#include <math.h>

using std::ios;
using std::cout;
using std::endl;

static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

int main(int argc, char** argv){
	image_t* img = image_readJPG(argv[1]);
	image_t* scaled = image_resize(img, 0.5);
	image_delete(img);
	image_delete(scaled);
	cout<<scaled->sizy<<" "<<scaled->sizx<<endl;

	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	facemodel_delete(model);

	return 0;
}
