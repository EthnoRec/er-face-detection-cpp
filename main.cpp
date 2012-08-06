#include "eHimage.h"
#include "eHmatrix.h"
#include "eHutils.h"
#include "eHfacemodel.h"
#include "eHfeatpyramid.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <iostream>
#include <fstream>

#include <string.h>
#include <math.h>

using std::ios;
using std::cout;
using std::endl;

mat3d_ptr eHhog(const image_ptr img, int sbin);

static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

int main(int argc, char** argv){
	//image_t* img = image_readJPG(argv[1]);
	image_t* img = image_readJPG("image/10081706_5.jpg");
	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	
	facepyra_t* pyra = facepyra_create(img,model);
	facepyra_delete(pyra);

	facemodel_delete(model);
	image_delete(img);

	return 0;
}
