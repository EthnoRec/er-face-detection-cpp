#include "eHimage.h"
#include "eHmatrix.h"
#include "eHutils.h"
#include "eHfacemodel.h"
#include "eHfeatpyramid.h"
#include "eHbox.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <sys/time.h>

#include <string.h>
#include <math.h>

#define millisecs(x) ((x).tv_sec*1000 + (x).tv_usec/1000)

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

#ifdef EH_TEST_TIMER
extern struct timeval time_spent_dp;
extern struct timeval time_spent_conv;
extern struct timeval time_spent_pyra;
extern struct timeval time_spent_detect;
#endif

int main(int argc, char** argv){
	image_t* img = NULL;
	if(argc<2)
		img = image_readJPG("image/10081706_5.jpg");
	else
		img = image_readJPG(argv[1]);
	//image_display(img,"test");
	
	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	vector<bbox_t> faces = facemodel_detect(img, model);

	facemodel_delete(model);
	image_delete(img);
#ifdef EH_TEST_TIMER
	cout<<"time_spent_detect: "<<millisecs(time_spent_detect)<<" ms"<<endl
		<<" - time_spent_pyra: "<<millisecs(time_spent_pyra)<<" ms"<<endl
		<<" - time_spent_conv: "<<millisecs(time_spent_conv)<<" ms"<<endl
		<<" - time_spent_dp: "<<millisecs(time_spent_dp)<<" ms"<<endl;
#endif

	return 0;
}
