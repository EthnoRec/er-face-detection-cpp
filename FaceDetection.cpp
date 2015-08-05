#include "FaceDetection.hpp"
#include <algorithm>
#include <ctime>


template<typename T>
Box<T>::Box(Point<T,2> o, Point<T,2> e) : origin(o), extent(e) {}

template<typename T>
Box<T>::Box(fbox_t b) : 
    origin(b.x1,b.y1),
    extent(b.x2,b.y2) {
}

template<typename T>
T Box<T>::area() const {
   return std::abs((origin.x  - extent.x)*(origin.y - extent.y)); 
}
template<typename T>
Box<T> Box<T>::merge(typename std::vector<Box<T>>::const_iterator start, typename std::vector<Box<T>>::const_iterator end) {
    Point<T,2> origin(std::numeric_limits<T>::max(),std::numeric_limits<T>::max());
    Point<T,2> extent(std::numeric_limits<T>::min(),std::numeric_limits<T>::min());

    for (auto it = start; it != end; it++) {
        auto box = *it;
        origin.x = std::min(origin.x, box.origin.x);
        origin.y = std::min(origin.y, box.origin.y);

        extent.x = std::max(extent.x, box.extent.x);
        extent.y = std::max(extent.y, box.extent.y);
    }
    return Box<T>(origin, extent);
}

FaceDetection::FaceDetection(bbox_t bb) : 
    score(bb.score),
    area(bb.area),
    component(bb.component),
    outer(bb.outer) {
        // std::vector<bbox_t> → std::vector<Box<double>>
        std::transform(
                bb.boxes.begin(),
                bb.boxes.end(),
                std::back_inserter(parts),
                [](fbox_t b){ return Box<int>(b); });

}

void FaceDetection::insert_none(pqxx::connection_base &c, const std::string image_id) {
    pqxx::work txn(c);
    
    std::time_t result = std::time(nullptr);
    std::stringstream dates_ss;
    dates_ss << "to_timestamp(" << result << "),to_timestamp(" << result << ")";
    std::string dates = dates_ss.str();

    std::stringstream fd_insert_ss;
    fd_insert_ss << "INSERT INTO \"FaceDetections\" (score,component,image_id,\"createdAt\",\"updatedAt\") VALUES ";
    fd_insert_ss << "(NULL,NULL," << txn.quote(image_id) << ","
                 << dates << ");";

    pqxx::result fd_r = txn.exec(fd_insert_ss.str());
    txn.commit();
}

void FaceDetection::insert(pqxx::connection_base &c, const std::string image_id) {
    pqxx::work txn(c);

    std::time_t result = std::time(nullptr);

    std::stringstream dates_ss;
    dates_ss << "to_timestamp(" << result << "),to_timestamp(" << result << ")";
    std::string dates = dates_ss.str();

    std::stringstream fd_insert_ss;
    fd_insert_ss << "INSERT INTO \"FaceDetections\" (score,component,image_id,\"createdAt\",\"updatedAt\") VALUES ";
    fd_insert_ss << "(" << score << "," << component << "," << txn.quote(image_id) << ","
                 << dates << ") RETURNING _id;";

    pqxx::result fd_r = txn.exec(fd_insert_ss.str());
    int fd_id = fd_r[0][0].as<int>();


    std::stringstream boxes_insert_ss;
    boxes_insert_ss << "INSERT INTO \"Boxes\"(origin_x,origin_y,extent_x,extent_y,fd_id,part_index,\"createdAt\",\"updatedAt\") VALUES ";

    for (auto it = parts.begin(); it != parts.end(); it++) {
        Box<int> part = *it;
        int part_index = it-parts.begin();
        boxes_insert_ss << "(" << part.origin.x << "," << part.origin.y << "," 
                        << part.extent.x << "," << part.extent.y << ","
                        << fd_id << "," << part_index << ","
                        << dates << "),";
    }

    boxes_insert_ss << "(" << outer.origin.x << "," << outer.origin.y << "," 
                 << outer.extent.x << "," << outer.extent.y << ","
                 << fd_id << ",NULL,"
                 << dates << ");";

    pqxx::result boxes_r = txn.exec(boxes_insert_ss);

    txn.commit();
}

Box<int> FaceDetection::left_eye() const {
    return Box<int>::merge(this->parts.begin()+9, this->parts.begin()+14);
}

Box<int> FaceDetection::right_eye() const {
    return Box<int>::merge(this->parts.begin()+20, this->parts.begin()+25);
}

std::ostream& operator<<(std::ostream& stream, 
        const FaceDetection& fd) {
    stream 
        << "Score: "
        << fd.score
        << " | Component: " 
        << fd.component 
        << " | Outer area: "
        << fd.area
        << " | No. of parts: "
        << fd.parts.size()
        << "\n Bounding box: "
        << fd.outer;
    return stream;
}

YAML::Emitter& operator<<(YAML::Emitter& stream, 
        FaceDetection const& fd) {
    stream 
        << YAML::Key << "score"
        << YAML::Value << fd.score

        << YAML::Key << "component"
        << YAML::Value << fd.component

        << YAML::Key << "outer"
        << YAML::Value << fd.outer

        << YAML::Key << "left_eye"
        << YAML::Value << fd.left_eye()

        << YAML::Key << "right_eye"
        << YAML::Value << fd.right_eye();
    return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, 
        const Box<T>& b) {
    // U+2192 : →
    stream << b.origin << " \u2192 " << b.extent
           << " [" << b.area() << "px\u00B2]";
    return stream;
}

YAML::Emitter& operator<<(YAML::Emitter& stream, 
        Boxi const& b) {
    stream 
        << YAML::BeginMap
        << YAML::Key << "origin"
        << YAML::Value << b.origin
        << YAML::Key << "extent"
        << YAML::Value << b.extent
        << YAML::EndMap;
    return stream;
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, 
        const Point<T,2>& p) {
    stream << "(" << p.x << "," << p.y << ")";
    return stream;
}

YAML::Emitter& operator<<(YAML::Emitter& stream, 
        Point2i const& p) {
    stream 
        << YAML::BeginSeq 
        << p.x
        << p.y
        << YAML::EndSeq;
    return stream;
}
