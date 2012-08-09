#ifndef EHFILTER_H
#define EHFILTER_H

#include "eHmatrix.h"

typedef struct eHfilter {
	int i;
	mat3d_t w;
} filter_t;

#endif
