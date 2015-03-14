#ifndef FACEDETECTION_H

#define FACEDETECTION_H
#include <vector>
#include <iostream>
#include "eHbbox.h"




template<typename T>
class Box {
    public:
        Box(T o, T e);
        Box(fbox_t b);
        std::pair<T,T> origin, extent;
};

class FaceDetection {
    public:
        FaceDetection(bbox_t bb);

        double score;
        double area;
        int component;

        std::vector<Box<double>> parts;

        Box<double> outer;

        friend std::ostream& operator<<(std::ostream& stream, 
        const FaceDetection& fd);
        
};

#endif /* end of include guard: FACEDETECTION_H */
