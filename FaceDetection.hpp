#ifndef FACEDETECTION_H

#define FACEDETECTION_H
#include <vector>
#include <iostream>
#include <memory>
#include <initializer_list>
#include "eHbbox.h"
#include "yaml-cpp/yaml.h"
#include <pqxx/pqxx>


// Point
template<typename T,int d> class Point;
typedef Point<int, 2> Point2i;

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Point<T,2>& b);
YAML::Emitter& operator<<(YAML::Emitter& stream, Point2i const& p);

template<typename T>
class Point<T,2> {
    public:
        T x, y;
        Point(T x, T y) : x(x), y(y) {}

        friend std::ostream& operator<< <>(std::ostream& stream, const Point& b);
};


// Box
template<typename T> class Box;
typedef Box<int> Boxi;

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Box<T>& b);

YAML::Emitter& operator<<(YAML::Emitter& stream, Boxi const& b);

template<typename T>
class Box {
    public:
        Box(Point<T,2> o, Point<T,2> e);
        Box(fbox_t b);
        static Box<T> merge(typename std::vector<Box<T>>::const_iterator, typename std::vector<Box<T>>::const_iterator);

        Point<T,2> origin, extent;

        T area() const;

        friend std::ostream& operator<< <>(std::ostream& stream, const Box<T>& b);
};


// FaceDetection
class FaceDetection {
    public:
        FaceDetection(bbox_t bb);
        static FaceDetection& none();

        Box<int> left_eye() const;
        Box<int> right_eye() const;

        double score;
        double area;
        int component;

        std::vector< Box<int> > parts;

        Box<int> outer;

        friend std::ostream& operator<<(std::ostream& stream, const FaceDetection& fd);
        friend YAML::Emitter& operator<<(YAML::Emitter& stream, FaceDetection const& fd);

        void insert(pqxx::connection_base &c, std::string image_id);
        static void insert_none(pqxx::connection_base &c, std::string image_id);
};

#endif /* end of include guard: FACEDETECTION_H */
