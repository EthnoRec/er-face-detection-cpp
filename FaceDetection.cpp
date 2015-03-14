#include "FaceDetection.hpp"
#include <algorithm>


template<typename T>
Box<T>::Box(T o, T e) : origin(o), extent(e) {}

template<typename T>
Box<T>::Box(fbox_t b) : 
    origin(b.x1,b.x2),
    extent(b.x2,b.y2) {
}

FaceDetection::FaceDetection(bbox_t bb) : 
    score(bb.score),
    area(bb.area),
    component(bb.component),
    outer(bb.outer) {
        // std::vector<bbox_t> -> std::vector<Box<double>>
        std::transform(
                bb.boxes.begin(),
                bb.boxes.end(),
                std::back_inserter(parts),
                [](fbox_t b){ return Box<double>(b); });

}


std::ostream& operator<<(std::ostream& stream, 
        const FaceDetection& fd) {
    stream 
        << "Score: "
        << fd.score
        << " | Component: " 
        << fd.component 
        << " | Outer area: " << fd.area;
    return stream;
}
