/*
 * eHfeatpyramid.h
 * Compute feature pyramid (HOG feature)
 *
 * Hang Su
 * 2012-08 @ eH
 */
#ifndef EHFEATPYRAMID_H
#define EHFEATPYRAMID_H

#include "eHmatrix.h"
#include "eHimage.h"
#include "eHfacemodel.h"
#include <vector>

using std::vector;

typedef struct eHfeatpyramid {
	vector<mat3d_t> feat;
	double* scale;
	int interval;
	int imy;
	int imx;
} facepyra_t;

facepyra_t* facepyra_create(image_t* im, facemodel_t* model);

void facepyra_delete(facepyra_t* pyra);

#endif
