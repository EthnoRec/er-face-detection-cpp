#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"

#include <iostream>
#include <vector>
#include <sys/time.h>

#define millisecs(x) ((x).tv_sec*1000 + (x).tv_usec/1000)

using std::cout;
using std::endl;
using std::vector;

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
	vector<bbox_t> faces = facemodel_detect(model,img);
	facemodel_delete(model);
	
	//posemodel_t* posemodel = posemodel_readFromFile("pose_BUFFY.xml");
	//vector<bbox_t> poses = posemodel_detect(posemodel,img);
	//posemodel_delete(posemodel);
	
	image_delete(img);
#ifdef EH_TEST_TIMER
	cout<<"time_spent_detect: "<<millisecs(time_spent_detect)<<" ms"<<endl
		<<" - time_spent_pyra: "<<millisecs(time_spent_pyra)<<" ms"<<endl
		<<" - time_spent_conv: "<<millisecs(time_spent_conv)<<" ms"<<endl
		<<" - time_spent_dp: "<<millisecs(time_spent_dp)<<" ms"<<endl;
#endif
	return 0;
}
