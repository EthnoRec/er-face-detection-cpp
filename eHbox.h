/*
 * eHbox.h
 *
 * just box ...
 */

#ifndef EHBOX_H
#define EHBOX_H

typedef struct eHbox_f {
	double x1;
	double y1;
	double x2;
	double y2;
} fbox_t;

typedef struct eHbox_i {
	int x1;
	int y1;
	int x2;
	int y2;
} ibox_t;

void fbox_set(fbox_t*, double x1, double y1, double x2, double y2);

void ibox_set(ibox_t*, int x1, int y1, int x2, int y2);

double fbox_interArea(const fbox_t& box1, const fbox_t& box2);

void fbox_clip(fbox_t& box, const int* imsize);

#endif
