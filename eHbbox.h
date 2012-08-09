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

#define EH_BBOXS_PRUNE 30000

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

/*
 * Non-maximum suppression
 * Greedily select high-scoring detections and skip detections that are 
 * significantly coverd by a previously selected detection
 */
void bboxv_nms(vector<bbox_t>& bboxes, double overlap, int prune = EH_BBOXS_PRUNE);

#endif
