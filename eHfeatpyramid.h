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

typedef struct eHfeatpyramid {
	mat3d_ptr* feat;
	double* scale;
	int len;
	int interval;
	int imy;
	int imx;
} facepyra_t;

facepyra_t* facepyra_create(image_t* im, facemodel_t* model);

void facepyra_delete(facepyra_t* pyra);

#endif
