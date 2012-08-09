/*
 * eHfilter.cpp
 *
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfilter.h"
#include <math.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <vector>
#include <iostream>

//#define EH_USE_CACHE

#define EH_CACHE_FILTERS_SIZE 120000
#define EH_CACHE_FEATURE_SIZE 5120000
#define EH_CACHE_FILTER_NUM 146

using std::vector;
using std::cerr;
using std::endl;

extern "C" {
#include "cblas.h"
}

struct thread_data {
	mat3d_t A;
	mat3d_t B;
	mat2d_t C;
};

#ifdef EH_USE_CACHE
static double cache_filters[EH_CACHE_FILTERS_SIZE];
static double cache_feature[EH_CACHE_FEATURE_SIZE];
static struct thread_data cache_threadData[EH_CACHE_FILTER_NUM];
static pthread_t cache_threads[EH_CACHE_FILTER_NUM];
#endif

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
  pthread_exit(NULL);
}

/*
 * entry point
 * resp = eHconv(cell of B, A, start, end);
 */
mat3d_ptr filterv_apply(const vector<filter_t> filters, const mat3d_ptr feats, int start, int end) {
 //void  eHconv(vector<mat2d_ptr>& resps, const mat3d_ptr feats, const vector<filter_t> filters, int start, int end) {

  int len = end-start+1;
  int filter_h = filters[0].w.sizy;
  int filter_w = filters[0].w.sizx;
  int filter_z = filters[0].w.sizz;
  int filter_len = filter_h*filter_w*filter_z;
  assert(len<=filters.size());
  int height = feats->sizy - filter_h + 1;
  int width = feats->sizx - filter_w + 1;
  assert(height>=1 && width>=1);
  mat3d_ptr resps= mat3d_alloc(height, width, filters.size());
  /*XXX necessary */
  for(int i=0;i<resps->sizx*resps->sizy*resps->sizz;i++)
	  resps->vals[i]=0;

  thread_data* td;
  pthread_t* ts;
  double *tmp_filter, *tmp_feats;
#ifdef EH_USE_CACHE
  if(len > EH_CACHE_FILTER_NUM) {
  	td = new thread_data[len];
 	ts = new pthread_t[len];
  } else {
  	td = cache_threadData;
	ts = cache_threads;
  }
  if(feats->sizy*feats->sizx*feats->sizz > EH_CACHE_FEATURE_SIZE)
	  tmp_feats = new double[feats->sizy*feats->sizx*feats->sizz];
  else
	  tmp_feats = cache_feature;
  if(filter_len*len > EH_CACHE_FILTERS_SIZE)
	  tmp_filter = new double[filter_len*len];
  else
	  tmp_filter = cache_filters;
#else
  	td = new thread_data[len];
 	ts = new pthread_t[len];
	tmp_feats = new double[feats->sizy*feats->sizx*feats->sizz];
	tmp_filter = new double[filter_len*len];
#endif

  prepare_map(tmp_feats,feats->vals,feats->sizy,feats->sizx,feats->sizz);

  for (int i = 0; i < len; i++) {
	  td[i].A.vals = tmp_feats;
	  td[i].A.sizy = feats->sizy;
	  td[i].A.sizx = feats->sizz;
	  td[i].A.sizz = feats->sizx;
	  td[i].B.vals = prepare_filter(tmp_filter+i*filter_len, filters[i].w.vals, 
		  filter_h, filter_w, filter_z);
	  td[i].B.sizy = filter_z;
	  td[i].B.sizx = filter_w;
	  td[i].B.sizz = filter_h;
	  td[i].C.vals = resps->vals+i*height*width;
	  td[i].C.sizy = height;
	  td[i].C.sizx = width;
	  
	  if(pthread_create(&ts[i], NULL, process, (void*)&td[i])){
		  /*error*/
		  cerr<<"Error creating thread!"<<endl;
		  break;
	  }
  }
  void* status;
  for (int i=0;i<len;i++){
	  pthread_join(ts[i],&status);
  }
#ifdef EH_USE_CACHE 
  if(feats->sizy*feats->sizx*feats->sizz > EH_CACHE_FEATURE_SIZE)
	  delete[] tmp_feats;
  if(filter_len*len > EH_CACHE_FILTERS_SIZE)
	  delete[] tmp_filter;
  if(len > EH_CACHE_FILTER_NUM) {
  	delete[] td;
  	delete[] ts;
  }
#else
  delete[] tmp_filter;
  delete[] tmp_feats;
  delete[] td;
  delete[] ts;
#endif
  return resps;
}

void *process_ST(void *thread_arg) {
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

mat3d_ptr filterv_apply_ST(const vector<filter_t> filters, const mat3d_ptr feats, int start, int end) {
  int len = end-start+1;
  int filter_h = filters[0].w.sizy;
  int filter_w = filters[0].w.sizx;
  int filter_z = filters[0].w.sizz;
  assert(len<=filters.size());
  int height = feats->sizy - filter_h + 1;
  int width = feats->sizx - filter_w + 1;
  assert(height>=1 && width>=1);
  mat3d_ptr resps= mat3d_alloc(height, width, filters.size());
  /*XXX necessary? */
  for(int i=0;i<resps->sizx*resps->sizy*resps->sizz;i++)
	  resps->vals[i]=0;

  thread_data td;
  double* tmp_feats = new double[feats->sizy*feats->sizx*feats->sizz];
  double* tmp_filter = new double[filter_h*filter_w*filter_z];
  prepare_map(tmp_feats,feats->vals,feats->sizy,feats->sizx,feats->sizz);

  for (int i = 0; i < len; i++) {
	  td.A.vals = tmp_feats;
	  td.A.sizy = feats->sizy;
	  td.A.sizx = feats->sizz;
	  td.A.sizz = feats->sizx;
	  td.B.vals = prepare_filter(tmp_filter, filters[i].w.vals, 
		  filter_h, filter_w, filter_z);
	  td.B.sizy = filter_z;
	  td.B.sizx = filter_w;
	  td.B.sizz = filter_h;
	  td.C.vals = resps->vals+i*height*width;
	  td.C.sizy = height;
	  td.C.sizx = width;
	  
	  process_ST((void *)&td);
  }
  delete[] tmp_feats;
  delete[] tmp_filter;
  return resps;
}

