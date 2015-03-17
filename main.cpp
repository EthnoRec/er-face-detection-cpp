#include <iostream>
#include <fstream>
#include <vector>
#include <pqxx/pqxx>
#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"
#include "FaceDetection.hpp"


class FaceDetector {
    public:
        FaceDetector(std::istream &is,
                std::string images_path, 
                pqxx::connection& con,
                std::string facemodel_path = "face_p146.xml") : 
            con(con), is(is), images_path(images_path) {
                facemodel = facemodel_readFromFile(facemodel_path.c_str());
            }

        void start() {
            for (std::string line; std::getline(is,line);) {

                std::size_t dotIndex = line.find(".");

                std::string path(images_path+"/"+line);

                if (dotIndex == std::string::npos || !fileExists(path)) {
                    continue;
                }                

                std::string id = line.substr(0,dotIndex);

                // WORK
                image_t* img = image_readJPG(path.c_str());
                std::vector<bbox_t> faces;
                faces = facemodel_detect(facemodel,img);
                // END WORK

                for(bbox_t face : faces) {
                    FaceDetection fd(face);

                    std::cout << fd << std::endl;
                    fd.insert(con,id);
                }
                std::cout << "_id:\t" << id << std::endl;
                std::cout << "faces:\t" << faces.size() << std::endl;
                std::cout << std::endl;
            }
        }

        ~FaceDetector() {
            facemodel_delete(facemodel);
        }

    private:
        // eH
        facemodel_t *facemodel;

        pqxx::connection& con;
        std::istream &is;
        std::string images_path;

        static bool fileExists(std::string p) {
            std::ifstream f(p);
            bool exists = f.good();
            f.close();
            return exists;
        }
};

// 1. read \n-separated list of <image_id>.<ext>s from stdin
// 2. locate the images in <image_dir> - first and only argument
// 3. for each path to image
//      insert <faces_detected> records into FaceDetections
//          for each FaceDetectiion
//              insert <parts_n> records into Boxes
//              insert 1 outer box record into Boxes
int main(int argc, const char *argv[])
{
    pqxx::connection con("host=127.0.0.1 dbname=tinder_development user=tinder password=tinder_pw");
    FaceDetector fd(std::cin,argv[1],con);
    fd.start();
    return 0;
}
