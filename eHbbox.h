/*
 * eHbbox.h
 *
 * Bounding box
 * XXX consider use Rect<> instead of fbox_t
 */
#ifndef EHBBOX_H
#define EHBBOX_H

#include "eHbox.h"
#include <vector>

using std::vector;

typedef struct eHbbox {
	vector<fbox_t> boxes;
	double score;
	int component;
	fbox_t outer;
	double area;
} bbox_t;

/*
 * filling the fields of given bbox: 
 * outer - bounding box covering all parts
 * area - area of outer
 */
void bbox_calcOut(bbox_t*);

/*
 * clip the boxes to image boundary
 */
void bbox_clipboxes(bbox_t& bbox, const int* imsize);

#endif
