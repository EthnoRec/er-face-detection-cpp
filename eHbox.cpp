#include "eHbox.h"

static inline double min(double x, double y) {return (x <= y ? x : y); }
static inline double max(double x, double y) {return (x <= y ? y : x); }

void setbox(fbox_t* box, double x1, double y1, double x2, double y2) {
	box->x1 = x1;
	box->y1 = y1;
	box->x2 = x2;
	box->y2 = y2;
}

void setbox(ibox_t* box, int x1, int y1, int x2, int y2) {
	box->x1 = x1;
	box->y1 = y1;
	box->x2 = x2;
	box->y2 = y2;
}

double intersect_area(const fbox_t box1, const fbox_t box2) {
	double w = max(0,min(box1.x2,box2.x2)-max(box1.x1,box2.x1));
	double h = max(0,min(box1.y2,box2.y2)-max(box1.y1,box2.y1));
	return w*h;
}
