#ifndef EHFILTER_H
#define EHFILTER_H

#include "eHmatrix.h"
#include <vector>

using std::vector;

typedef struct eHfilter {
	int i;
	mat3d_t w;
} filter_t;

/* Convolve a feature map with a set of filters
 * Multithreaded version
 * usage:
 * 	mat3d_ptr resps = 
 * 	eHconv(vector<filter_t>filters, mat3d_ptr feats, int start, int end) 
 * 	feats - feature map 
 * 	filters - a set of part filters
 * 	start, end - range of filters used
 * NOTE: 
 * 	(1)all matrices/images are in column-major style (as in Fortran & Matlab)
 * 	(2)mat3d_ptr resps is allocated inside, proper delete is necessary after use
 * 	(3)cblas library is required
 */
mat3d_ptr filterv_apply(const vector<filter_t> filters, const mat3d_ptr feats, int start, int end);

#endif
