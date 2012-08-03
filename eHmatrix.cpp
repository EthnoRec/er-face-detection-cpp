/*
 * eHmatrix.cpp
 *
 * Hang Su
 * 2012-07 @ eH
 */
#include "eHmatrix.h"

mat2d_ptr mat2d_alloc(size_t sizy, size_t sizx) {
	mat2d_t* mat = new mat2d_t;
	mat->vals = new double[sizy*sizx];
	mat->sizy = sizy;
	mat->sizx = sizx;
	return mat;
}

void mat2d_delete(mat2d_ptr mat) {
	delete[] mat->vals;
	delete[] mat;
}

mat3d_ptr mat3d_alloc(size_t sizy, size_t sizx, size_t sizz) {
	mat3d_t* mat = new mat3d_t;
	mat->vals = new double[sizy*sizx*sizz];
	mat->sizy = sizy;
	mat->sizx = sizx;
	mat->sizz = sizz;
	return mat;
}

void mat3d_delete(mat3d_ptr mat) {
	delete[] mat->vals;
	delete[] mat;
}

matkd_ptr matkd_alloc(size_t k, size_t* sizs) {
	matkd_t* mat = new matkd_t;
	mat->k = k;
	mat->siz = new size_t[k];
	int len = 1;
	for (int i=0;i<k;i++) {
		len*=sizs[i];
		mat->siz[i]=sizs[i];
	}
	mat->vals = new double[len];
	return mat;
}

void matkd_delete(matkd_ptr mat) {
	delete[] mat->siz;
	delete[] mat->vals;
	delete[] mat;
}

