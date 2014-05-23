#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"
#include <cstring>
#include <vector>
#include <iostream>

int main(int argc, char** argv){

	if(argc==2) {
		//load face model & body model
		facemodel_t* facemodel = facemodel_readFromFile("face_p146.xml");
		posemodel_t* posemodel = posemodel_readFromFile("pose_BUFFY.xml");

		//load a jpeg image
		image_t* img = image_readJPG(argv[1]);
		if(NULL==img) {
			std::cout<<"Error: cannot open "<<argv[1]<<std::endl;
			facemodel_delete(facemodel);
			posemodel_delete(posemodel);
			return 0;
		}
	
		//detect faces and show results
		std::vector<bbox_t> faces = facemodel_detect(facemodel,posemodel,img);
		image_showDetection(img, faces, "Face Detection Results");
	
		//destruct image and models
		image_delete(img);
		facemodel_delete(facemodel);
		posemodel_delete(posemodel);
	} else if (argc==6) {
		//load face model & body model
		facemodel_t* facemodel = facemodel_readFromFile(argv[2]);
		posemodel_t* posemodel = posemodel_readFromFile(argv[3]);

		//load a jpeg image
		image_t* img = image_readJPG(argv[1]);
		if(NULL==img) {
			std::cout<<"Error: cannot open "<<argv[1]<<std::endl;
			facemodel_delete(facemodel);
			posemodel_delete(posemodel);
			return 0;
		}

		//detect faces and show results
		std::vector<bbox_t> faces;
		if(NULL==posemodel)
			faces = facemodel_detect(facemodel,img);
		else
			faces = facemodel_detect(facemodel,posemodel,img);
		if(0!=strcmp(argv[4],"-")) 
			image_writeDetectionJpg(img, faces, argv[4]);
		if(0!=strcmp(argv[5],"-"))
			image_writeDetectionXml(faces, argv[5]);
	
		//destruct image and models
		image_delete(img);
		facemodel_delete(facemodel);
		posemodel_delete(posemodel);
	} else {
		std::cout<<"Usage 1: facefinder <image>"<<std::endl
		<<"Usage 2: facefinder <image> <faceModel> <poseModel> <jpgSaveDir> <xmlSaveDir>"<<std::endl
		<<" (use '-' for any unwanted parameters)"<<std::endl;
	}

	return 0;
}
