#include "eHimage.h"
#include "eHutils.h"
#include "eHfacemodel.h"

#include "rapidxml-1.13/rapidxml.hpp"

#include <iostream>
#include <fstream>

#include <string.h>

using std::ios;

int main(){
	image_ptr img = image_alloc(10,10);
	image_ptr scaled = image_resize(img,0.5);
	image_ptr reduced = image_reduce(img);
	image_delete(img);
	image_delete(scaled);

	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	facemodel_delete(model);

	return 0;
}
