#ifndef FACEDETECTION_H

#define FACEDETECTION_H
#include <vector>
#include <iostream>
#include <memory>
#include <initializer_list>
#include "eHbbox.h"
//#include <odb/core.hxx>
#include <pqxx/pqxx>


template<typename T,int d> class Point;

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Point<T,2>& b);

template<typename T>
class Point<T,2> {
    public:
        T x, y;
        Point(T x, T y) : x(x), y(y) {}

        friend std::ostream& operator<< <>(std::ostream& stream, const Point& b);
};

template<typename T> class Box;
template<typename T>
std::ostream& operator<<(std::ostream& stream, const Box<T>& b);

template<typename T>
class Box {
    public:
        Box(T o, T e);
        Box(fbox_t b);
        Point<T,2> origin, extent;

        T area() const;

        friend std::ostream& operator<< <>(std::ostream& stream, const Box<T>& b);
};


class FaceDetection {
    public:
        FaceDetection(bbox_t bb);
        static FaceDetection& none();
        double score;
        double area;
        int component;

        std::vector< Box<int> > parts;

        Box<int> outer;

        friend std::ostream& operator<<(std::ostream& stream, const FaceDetection& fd);

        void insert(pqxx::connection_base &c, std::string image_id);
        static void insert_none(pqxx::connection_base &c, std::string image_id);
};

#endif /* end of include guard: FACEDETECTION_H */
