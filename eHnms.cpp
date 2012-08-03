/*
 * eHnms.cpp
 *
 * Non-maximum suppression
 * Greedily select high-scoring detections and skip detections that are 
 * significantly coverd by a previously selected detection
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHbbox.h"
#include <vector>
#include <list>
#include <algorithm>

#define MAX_BOXES_NUM 30000

using std::vector;
using std::list;

bool compare_score(bbox_t a, bbox_t b) {return (a.score>b.score);}

void nms(vector<bbox_t>& bboxes, double overlap) {
	if (bboxes.empty())
		return ;
	/* sort bboxes according to score */
	sort(bboxes.begin(), bboxes.end(), compare_score);

	/* throw away boxes with low score if there are too many */
	if(bboxes.size()>MAX_BOXES_NUM) 
		bboxes.resize(MAX_BOXES_NUM);

	/* get bounding box & area */
	vector<bbox_t>::iterator iter;
	for(iter=bboxes.begin();iter!=bboxes.end();iter++)
		calcOut_bbox(&(*iter));

	/* use list for frequent removal */
	list<bbox_t> lsbboxes(bboxes.begin(),bboxes.end());
	list<bbox_t>::iterator iter_l1, iter_l2;
	double intersect;
	iter_l1 = lsbboxes.begin();
	while(iter_l1!=lsbboxes.end()){
		iter_l2 = iter_l1;
		iter_l2++;
		while(iter_l2!=lsbboxes.end()){
			intersect = intersect_area(iter_l1->outer, iter_l2->outer);
			if((intersect/iter_l1->area)>overlap || (intersect/iter_l2->area)>overlap) {
				lsbboxes.erase(iter_l2);
				continue;
			}
			iter_l2++;
		}
		iter_l1++;
	}
	/* copy results back to vector*/
	bboxes.resize(lsbboxes.size());
	copy(lsbboxes.begin(),lsbboxes.end(),bboxes.begin());
}
