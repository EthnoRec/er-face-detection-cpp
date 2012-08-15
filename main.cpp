#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"
#include <vector>

int main(int argc, char** argv){
	//load face model & body model
	facemodel_t* facemodel = facemodel_readFromFile("face_p146.xml");
	posemodel_t* posemodel = posemodel_readFromFile("pose_BUFFY.xml");

	//load an jpeg image
	image_t* img = image_readJPG(argv[1]);
	
	//detect faces and show results
	std::vector<bbox_t> faces = facemodel_detect(facemodel,posemodel,img);
	image_showDetection(img, faces, "Face Detection Results");
	
	//destruct image and models
	image_delete(img);
	facemodel_delete(facemodel);
	posemodel_delete(posemodel);

	return 0;
}
