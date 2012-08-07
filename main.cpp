#include "eHimage.h"
#include "eHmatrix.h"
#include "eHutils.h"
#include "eHfacemodel.h"
#include "eHfeatpyramid.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <iostream>
#include <fstream>
#include <vector>

#include <string.h>
#include <math.h>

using std::ios;
using std::cout;
using std::endl;
using std::vector;

mat3d_ptr eHconv(const mat3d_ptr feats, const vector<facefilter_t> filters, int start, int end);
mat3d_ptr eHhog(const image_ptr img, int sbin);
void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		double* vals, int sizx, int sizy, 
		double ax, double bx, double ay, double by);
	

static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

int main(int argc, char** argv){
	//image_t* img = image_readJPG(argv[1]);
	image_t* img = image_readJPG("image/10081706_5.jpg");
	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	
	//facepyra_t* pyra = facepyra_create(img,model->interval,model->sbin,model->maxsize);
	//mat3d_ptr resp = eHconv(pyra->feat[0],model->filters,0,145);
	//mat3d_delete(resp);
	//facepyra_delete(pyra);
	vector<bbox_t> boxes = facemodel_detect(img, model);
	

	facemodel_delete(model);
	image_delete(img);

	return 0;
}
