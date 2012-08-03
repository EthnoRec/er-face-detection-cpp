#include "eHbbox.h"

#define IMG_BOUND 10000

static inline double min(double x, double y) {return (x <= y ? x : y); }
static inline double max(double x, double y) {return (x <= y ? y : x); }

/*
 * filling the fields of given bbox: 
 * outer - bounding box covering all parts
 * area - area of outer
 */
void calcOut_bbox(bbox_t* bbox){
	vector<fbox_t>::iterator iter;
	setbox(&(bbox->outer),IMG_BOUND,IMG_BOUND,-IMG_BOUND,-IMG_BOUND);
	for(iter=bbox->boxes.begin();iter!=bbox->boxes.end();iter++)
		setbox(&(bbox->outer),
				min(bbox->outer.x1,iter->x1),
				min(bbox->outer.y1,iter->y1),
				max(bbox->outer.x2,iter->x2),
				max(bbox->outer.y2,iter->y2)
		      );
	bbox->area = max(0,(bbox->outer.x2-bbox->outer.x1))
		*max(0,(bbox->outer.y2-bbox->outer.y1));
}
