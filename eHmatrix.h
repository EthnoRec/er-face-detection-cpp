/*
 * eHmatrix.h
 * Basic matrix types and operations
 *
 * Hang Su
 * 2012-07 @ eH
 */
#ifndef EHMATRIX_H
#define EHMATRIX_H

#include <stdlib.h>

typedef struct eHmatrix2d {
	double* vals;
	size_t sizy;
	size_t sizx;
} mat2d_t;
typedef mat2d_t* mat2d_ptr;

typedef struct eHmatrix3d {
	double* vals;
	size_t sizy;
	size_t sizx;
	size_t sizz;
} mat3d_t;
typedef mat3d_t* mat3d_ptr;

typedef struct eHmatrixkd {
	double* vals;
	size_t k;
	size_t* siz;
} matkd_t;
typedef matkd_t* matkd_ptr;

mat2d_ptr mat2d_alloc(size_t sizy, size_t sizx);

/* this should be used only for mat2d allocated with mat2d_alloc() */
void mat2d_delete(mat2d_ptr);

mat3d_ptr mat3d_alloc(size_t sizy, size_t sizx, size_t sizz);

/* this should be used only for mat2d allocated with mat3d_alloc() */
void mat3d_delete(mat3d_ptr);

matkd_ptr matkd_alloc(size_t k, size_t* sizs);

/* this should be used only for mat2d allocated with matkd_alloc() */
void matkd_delete(matkd_ptr);

#endif
