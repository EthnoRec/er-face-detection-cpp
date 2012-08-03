/*
 * eHfeatpyramid.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfeatpyramid.h"
#include <math.h>

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }
static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

facepyra_t* facepyra_create(image_t* im, facemodel_t* model) {
	facepyra_t* pyra = new facepyra_t;
	int interval = model->interval;
	int sbin = model->sbin;

	/* select padding, allowing for one cell in model to be visible
	 * Even padding allows for consistent spatial relations across 2X scales */
	int pady = max(model->maxsize[0]-1-1,0);
	int padx = max(model->maxsize[1]-1-1,0);
	double sc = pow(2.0, 1.0/(double)interval);

	int min_level = floor(log((double)min(im->sizy,im->sizx)/(5.0*sbin))/log(sc));
	
	return pyra;
}

void facepyra_delete(facepyra_t* pyra) {
	delete[] pyra->scale;
	delete pyra;
}


