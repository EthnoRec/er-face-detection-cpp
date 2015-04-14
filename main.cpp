#include <iostream>
#include <fstream>
#include <vector>
#include <pqxx/pqxx>
#include <algorithm>
#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"
#include "FaceDetection.hpp"
#include "yaml-cpp/yaml.h"


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

                if (faces.empty()) {
                    FaceDetection::insert_none(con,id);
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

class ConfigParser {
    public:
        ConfigParser(const std::string filename) {
            YAML::Node config = YAML::LoadFile(filename);
            const YAML::Node db = config["database"];
            images = config["images"].as<std::string>();

            std::vector<std::string> db_props({"host","dbname","user","password"});
            std::string con_str("");
            for (std::size_t i = 0; i < db_props.size(); i++) {
                const std::string key = db_props[i];
                con_str += key + "=" + db[key].as<std::string>();
                if (i < db_props.size() - 1) {
                    con_str += " ";
                }
            }
            this->con_str = con_str;
        }
        std::string images;
        std::string con_str;
};

// 1st argument - config file

// 1. read \n-separated list of <image_id>.<ext>s from stdin
// 2. locate the images in <images>
// 3. for each path to image
//      insert <faces_detected> records into FaceDetections
//          for each FaceDetectiion
//              insert <parts_n> records into Boxes
//              insert 1 outer box record into Boxes
int main(int argc, const char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <valid-yaml-config-file>" << std::endl;
        return 1;
    }

    try {
        const std::string filename(argv[1]);
        ConfigParser cp(filename);
        pqxx::connection con(cp.con_str);
        FaceDetector fd(std::cin,cp.images,con);
        fd.start();
    } catch(YAML::BadFile e) {
        std::cerr << "Bad YAML configuration file." << std::endl; 
    }
    return 0;
}
