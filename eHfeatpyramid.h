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

typedef struct eHfeatpyramid {
	mat3d_ptr* feat;
	double* scale;
	int len; /*levels of pyra, size of feat & scale*/
	int interval;
	int imy;
	int imx;
} featpyra_t;

featpyra_t* featpyra_create(const image_ptr im, int interval, int sbin, const int* maxsize, bool hallucinate=true);

void featpyra_delete(featpyra_t* pyra);

#endif
