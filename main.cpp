#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"
#include "FaceDetection.hpp"
#include <cstring>
#include <vector>
#include <iostream>
#include <chrono>
#include <pqxx/pqxx>


int main(int argc, char** argv) {

    const char *imgPath, *facemodelPath, *posemodelPath, *jpgSavePath, *xmlSavePath;
    switch(argc) {
    case 2:
        imgPath = argv[1];
        facemodelPath = "face_p146.xml";
        posemodelPath = "pose_BUFFY.xml";
        jpgSavePath = "-";
        xmlSavePath = "-";
        break;
    case 6:
        imgPath = argv[1];
        facemodelPath = argv[2];
        posemodelPath = argv[3];
        jpgSavePath = argv[4];
        xmlSavePath = argv[5];
        break;
    default:
        std::cout<<"Usage 1: facefinder <image>"<<std::endl
                 <<"Usage 2: facefinder <image> <faceModel> <poseModel> <jpgSaveDir> <xmlSaveDir>"<<std::endl
                 <<" (use '-' for any unwanted parameters)"<<std::endl;
        return 0;
    }

    //load face model & body model
    facemodel_t* facemodel = facemodel_readFromFile(facemodelPath);
    posemodel_t* posemodel = posemodel_readFromFile(posemodelPath);

     auto start = std::chrono::steady_clock::now();
    //load a jpeg image
    image_t* img = image_readJPG(imgPath);
    if(NULL==img) {
        std::cout<<"Error: cannot open "<<imgPath<<std::endl;
        facemodel_delete(facemodel);
        posemodel_delete(posemodel);
        return 0;
    }

    //detect faces and show results
    std::vector<bbox_t> faces;
    if(true || NULL==posemodel)
        faces = facemodel_detect(facemodel,img);
    else
        faces = facemodel_detect(facemodel,posemodel,img);
    if(0!=strcmp(jpgSavePath,"-"))
        image_writeDetectionJpg(img, faces, jpgSavePath);
    if(0!=strcmp(xmlSavePath,"-"))
        image_writeDetectionXml(faces, xmlSavePath);
    auto end = std::chrono::steady_clock::now();





                
    pqxx::connection c("host=127.0.0.1 dbname=tinder user=tinder password=tinder_pw");

    for(bbox_t face : faces) {
        FaceDetection fd(face);

        std::cout << fd << std::endl;
        fd.insert(c,std::string("test"));
    }

    if(0==strcmp(jpgSavePath,"-") && 0==strcmp(xmlSavePath,"-")) {
        //image_showDetection(img,faces,"Face Detection Results");
        image_showFaces(img,faces,"Face Detection Results");
    }

    //destruct image and models
    image_delete(img);

    std::cout << std::chrono::duration <double, std::milli> (end-start).count()/1000.0 << " s" << std::endl;

    facemodel_delete(facemodel);
    posemodel_delete(posemodel);

    return 0;
}
