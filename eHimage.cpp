/*
 * eHimage.cpp
 *
 * Hang Su
 * 2012-07 @ eH
 */
#include "eHimage.h"
#include <assert.h>
#include <string.h>

static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

image_ptr image_alloc(size_t sizy, size_t sizx){
	image_ptr img = new struct eHimage;
	img->sizy = sizy;
	img->sizx = sizx;
	img->ch[0] = new double[sizy*sizx];
	img->ch[1] = new double[sizy*sizx];
	img->ch[2] = new double[sizy*sizx];
	return img;
}

void image_delete(image_ptr img){
	delete[] img->ch[0];
	delete[] img->ch[1];
	delete[] img->ch[2];
	delete img;
}

/* struct used for caching interpolation values */
/* used by image_resize() */
struct alphainfo {
	int si, di;
	double alpha;
};

/* copy src into dst using pre-computed interpolation values */
/* used by image_resize() */
void alphacopy(double* src, double*dst, struct alphainfo *ofs, int n) {
	struct alphainfo *end = ofs+n;
	while(ofs != end) {
		dst[ofs->di] += ofs->alpha * src[ofs->si];
		ofs++;
	}
}

/* resize along each column (result is transposed) */
/* used by image_resize() */
void resize1dtran(image_ptr src, size_t sheight, 
		image_ptr dst, size_t dheight, size_t width) {
	double scale = (double)dheight/(double)sheight;
	double invscale = (double)sheight/(double)dheight;

	/* cache interpolation values since they can be shared 
	 * among different columns*/
	int len = (int)ceil(dheight*invscale) + 2*dheight;
	alphainfo ofs[len];
	int k = 0;
	for (int dy=0;dy<dheight;dy++) {
		double fsy1 = dy * invscale;
		double fsy2 = fsy1 + invscale;
		int sy1 = (int)ceil(fsy1);
		int sy2 = (int)floor(fsy2);
		if(sy1-fsy1 > 1e-3) {
			assert(k<len);
			//assert(sy-1 >= 0);
			ofs[k].di = dy*width;
			ofs[k].si = sy1-1;
			ofs[k++].alpha = (sy1-fsy1)*scale;
		}
		for (int sy = sy1;sy<sy2;sy++) {
			assert(k<len);
			assert(sy<sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy;
			ofs[k++].alpha = scale;
		}
		if(fsy2-sy2 > 1e-3) {
			assert(k<len);
			assert(sy2<sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy2;
			ofs[k++].alpha = (fsy2-sy2)*scale;
		}
	}
	for (int nch = 0; nch<3; nch++) {
		memset(dst->ch[nch], 0, width*dheight*sizeof(double));
		for (int x = 0; x<width; x++) {
			double *s = src->ch[nch] + x*sheight;
			double *d = dst->ch[nch] + x;
			alphacopy(s,d,ofs,k);
		}
	}
}

/*
 * Fast image subsampling
 * Important: src image is not destroyed
 * This is used to construct the feature pyramid
 */
image_ptr image_resize(const image_ptr img, double scale) {
	size_t dst_sizy = (unsigned int)round2int(img->sizy*scale);
	size_t dst_sizx = (unsigned int)round2int(img->sizx*scale);
	image_ptr scaled = image_alloc(dst_sizy, dst_sizx);
	image_ptr tmp = image_alloc(img->sizx,dst_sizy);

	/* scale in columns, and transposed */
	resize1dtran(img,img->sizy,tmp,dst_sizy, img->sizx);
	/* scale in (old)rows, and transposed back */
	resize1dtran(tmp,img->sizx,scaled,dst_sizx,dst_sizy);

	image_delete(tmp);
	return scaled;
}

/* reduce along each column (result is transposed) */
/* used by image_reduce() */
void reduce1dtran(image_ptr src, size_t sheight, 
		image_ptr dst, size_t dheight, size_t width) {
	double *s, *d;
	for (int nch = 0; nch<3; nch++) {
		memset(dst->ch[nch], 0, width*dheight*sizeof(double));
		for (int x = 0; x<width; x++) {
			s = src->ch[nch] + x*sheight;
			d = dst->ch[nch] + x;

			/* First row */
			*d = s[0]*0.6875 + s[1]*0.2500 + s[2]*0.0625;
			
			/* middle rows */
			for (int y = 1; y<dheight-2;y++) {
				s += 2;
				d += width;
				*d = s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 
					+ s[1]*0.25 + s[2]*0.0625;
			}

			/* Last two rows */
			s += 2;
			d += width;
			if (dheight*2 <= sheight) {
				*d = s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 
					+ s[1]*0.25 + s[2]*0.0625;
			} else {
				*d = s[1]*0.3125 + s[0]*0.375 + s[-1]*0.25 
					+ s[-2]*0.0625;
			}
			s += 2;
			d += width;
			*d = s[0]*0.6875 + s[-1]*0.25 + s[-2]*0.0625;
		}
	}
}

/*
 * Reduce size to half, using 5-tap binomial filter for anti-aliasing
 * (see Burt & Adelson's Laplacian Pyramid paper for details)
 */
image_ptr image_reduce(const image_ptr img) {
	size_t dst_sizy = (unsigned int)round2int(img->sizy*.5);
	size_t dst_sizx = (unsigned int)round2int(img->sizx*.5);
	image_ptr scaled = image_alloc(dst_sizy, dst_sizx);
	image_ptr tmp = image_alloc(img->sizx,dst_sizy);

	/* scale in columns, and transposed */
	reduce1dtran(img,img->sizy,tmp,dst_sizy,img->sizx);
	/* scale in (old)columns, and transposed back */
	reduce1dtran(tmp,img->sizx,scaled,dst_sizx,dst_sizy);

	image_delete(tmp);
	return scaled;
}
