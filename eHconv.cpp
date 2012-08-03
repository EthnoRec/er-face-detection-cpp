/*
 * eHconv.cpp
 *
 * Convolve a feature map with a set of filters
 * usage:
 * 	mat3d_ptr resps = 
 * 	eHconv(mat3d_ptr feats, matkd_ptr filters, int start, int end) 
 * 	feats - feature map 
 * 	filters - a set of part filters
 * 	start, end - range of filters used
 * NOTE: 
 * 	(1)all matrices/images are in column-major style (as in Fortran & Matlab)
 * 	(2)mat3d_ptr resps is allocated inside, proper delete is necessary after use
 * 	(3)cblas library is required
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHmatrix.h"
#include <math.h>
#include <string.h>
#include <assert.h>

extern "C" {
#include "cblas.h"
}

struct thread_data {
	mat3d_t A;
	mat3d_t B;
	mat2d_t C;
};

/* reshape matrix
 * (Old)y x z ==> (F)z x y
 * NOTE: result is stored in F, which should be allocated already
 */
double *prepare_filter(double* F, double* Old, size_t sizy, size_t sizx, size_t sizz) {  
	for (int f = 0; f < sizz; f++) {
		for (int x = 0; x < sizx; x++) {
			for (int y = 0; y < sizy; y++) {
				F[f + x*(sizz) + y*(sizx*sizz)] =  
					Old[y + x*sizy + f*(sizy*sizx)];
			}
		}
	}
	return F;
}

/* reshape matrix
 * (Old)y x z ==> (F)y z x
 * NOTE: result is stored in F, which should be allocated already
 */
double *prepare_map(double* F, double* Old, size_t sizy, size_t sizx, size_t sizz) {  
	for (int f = 0; f < sizz; f++) {
		for (int x = 0; x < sizx; x++) {
			for (int y = 0; y < sizy; y++) {
				F[y + f*sizy + x*(sizy*sizz)] =  
					Old[y + x*sizy + f*(sizy*sizx)];
			}
		}
	}
	return F;
}

/* convolve A and B using cblas */
void *process(void *thread_arg) {
  thread_data *args = (thread_data *)thread_arg;
  double *A = args->A.vals;
  double *B = args->B.vals;
  double *C = args->C.vals;

  for (int x = 0; x < args->C.sizx; x++) {
    for (int y = 0; y < args->B.sizz; y++) {
      double *A_off = A + x*(args->A.sizy*args->A.sizx) + y;
      double *B_off = B + y*(args->B.sizy*args->B.sizx);
      double *C_off = C + x*(args->C.sizy);
      double one = 1;
      int m = args->C.sizy;
      int n = args->B.sizy*args->B.sizx;
      int lda = args->A.sizy;
      int incx = 1;
      int incy = 1;
      cblas_dgemv(CblasColMajor,CblasNoTrans, m, n, one, A_off, lda, B_off, incx, one, C_off, incy);
    }
  }
}

/*
 * entry point
 * resp = eHconv(A, cell of B, start, end);
 */
mat3d_ptr eHconv(mat3d_ptr feats, matkd_ptr filters, int start, int end) {

  int len = end-start+1;
  int filter_len = filters->siz[0]*filters->siz[1]*filters->siz[2];
  assert(len<=filters->siz[3]);
  int height = feats->sizy - filters->siz[0] + 1;
  int width = feats->sizx - filters->siz[1] + 1;
  assert(height>=1 && width>=1);
  mat3d_ptr resps= mat3d_alloc(height, width, filters->siz[3]);

  thread_data td;
  double* tmp_feats = new double[feats->sizy*feats->sizx*feats->sizz];
  double* tmp_filter = new double[filters->siz[0]*filters->siz[1]*filters->siz[2]];
  prepare_map(tmp_feats,feats->vals,feats->sizy,feats->sizx,feats->sizz);

  for (int i = 0; i < len; i++) {
	  td.A.vals = tmp_feats;
	  td.A.sizy = feats->sizy;
	  td.A.sizx = feats->sizz;
	  td.A.sizz = feats->sizx;
	  td.B.vals = prepare_filter(tmp_filter, filters->vals+i*filter_len, 
		  filters->siz[0], filters->siz[1], filters->siz[2]);
	  td.B.sizy = filters->siz[2];
	  td.B.sizx = filters->siz[1];
	  td.B.sizz = filters->siz[0];
	  td.C.vals = resps->vals+i*height*width;
	  td.C.sizy = height;
	  td.C.sizx = width;
	  
	  process((void *)&td);
  }
  delete[] tmp_feats;
  delete[] tmp_filter;
}

