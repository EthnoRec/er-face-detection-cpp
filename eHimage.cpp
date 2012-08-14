/*
 * eHimage.cpp
 *
 * Hang Su
 * 2012-07 @ eH
 */
#include "eHimage.h"
#include <assert.h>
#include <string.h>

//#include "CImg-1.5.0/CImg.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

static inline int round2int(double x) { return ((x-floor(x))>0.5 ? (int)ceil(x) : (int)floor(x));}

image_ptr image_alloc(size_t sizy, size_t sizx, size_t nch){
	image_ptr img = new struct eHimage;
	img->sizy = sizy;
	img->sizx = sizx;
	img->nchannel = nch;
	img->data = new double[sizy*sizx*nch];
	for(unsigned i=0;i<nch;i++) {
		img->ch[i] = img->data + i*sizy*sizx;
	}
	img->is_shared = false;
	img->stepy = img->sizy;
	img->stepyx = img->sizy*img->sizx;
	return img;
}

void image_delete(image_ptr img){
	if(!img->is_shared)
		delete[] img->data;
	delete img;
}

/*
image_ptr image_readColorImage(const char* filename) {
	using namespace cimg_library;
	CImg<unsigned char> img(filename);
	assert(img.spectrum()==3);
	image_ptr im = image_alloc(img.height(),img.width());
	for(int y=0;y<img.height();y++){
		for(int x=0;x<img.width();x++){
			im->ch[0][y+x*img.height()] = (double)*img.data(x,y,0);
			im->ch[1][y+x*img.height()] = (double)*img.data(x,y,1);
			im->ch[2][y+x*img.height()] = (double)*img.data(x,y,2);
		}
	}
	return im;
}

cimg_library::CImgDisplay* image_display(const image_ptr img, const char* winname) {

	using namespace cimg_library;
	CImg<unsigned char> im(img->sizx,img->sizy,1,3);
	for (int y=0; y<im.height();y++) {
		for(int x=0;x<im.width();y++) {
			*im.data(x,y,0) = (unsigned char)img->ch[0][y+x*img->sizy];
			*im.data(x,y,1) = (unsigned char)img->ch[1][y+x*img->sizy];
			*im.data(x,y,2) = (unsigned char)img->ch[2][y+x*img->sizy];
		}
	}
	CImgDisplay* dispwin = new CImgDisplay(im,winname);
	return dispwin;
}
*/
image_ptr image_readJPG(const char* filename) {
	using namespace cv;
	Mat img = imread(filename, 1);
	if(!img.data) {
		return NULL;
	}
	image_ptr im = image_alloc(img.size().height, img.size().width);
	for(unsigned y=0;y<im->sizy;y++) {
		for(unsigned x=0;x<im->sizx;x++) {
			im->ch[0][y+x*im->stepy]=img.at<Vec3b>(y,x).val[2];
			im->ch[1][y+x*im->stepy]=img.at<Vec3b>(y,x).val[1];
			im->ch[2][y+x*im->stepy]=img.at<Vec3b>(y,x).val[0];
		}
	}

	return im;
}

void image_display(const image_ptr img, const std::string& winname) {
	using namespace cv;
	Mat M(img->sizy,img->sizx,CV_8UC3);
	for(unsigned int y=0;y<img->sizy;y++) {
		for(unsigned int x=0;x<img->sizx;x++) {
			M.at<Vec3b>(y,x)[2]=img->ch[0][y+x*img->stepy];
			M.at<Vec3b>(y,x)[1]=img->ch[1][y+x*img->stepy];
			M.at<Vec3b>(y,x)[0]=img->ch[2][y+x*img->stepy];
		}
	}
	namedWindow(winname, CV_WINDOW_AUTOSIZE);
	imshow(winname,M);
	waitKey();
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
	for (unsigned dy=0;dy<dheight;dy++) {
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
			assert(sy<(int)sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy;
			ofs[k++].alpha = scale;
		}
		if(fsy2-sy2 > 1e-3) {
			assert(k<len);
			assert(sy2<(int)sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy2;
			ofs[k++].alpha = (fsy2-sy2)*scale;
		}
	}
	for (int nch = 0; nch<3; nch++) {
		memset(dst->ch[nch], 0, width*dheight*sizeof(double));
		for (unsigned x = 0; x<width; x++) {
			double *s = src->ch[nch] + x*src->stepy;
			double *d = dst->ch[nch] + x;
			alphacopy(s,d,ofs,k);
		}
	}
}

/** @brief Fast image subsampling
 *  @note src image is not destroyed
 */
image_ptr image_resize(const image_ptr img, double scale) {
	size_t dst_sizy = (unsigned int)round2int(img->sizy*scale);
	size_t dst_sizx = (unsigned int)round2int(img->sizx*scale);
	image_ptr scaled = image_alloc(dst_sizy, dst_sizx, img->nchannel);
	image_ptr tmp = image_alloc(img->sizx,dst_sizy, img->nchannel);

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
		for (unsigned x = 0; x<width; x++) {
			s = src->ch[nch] + x*src->stepy;
			d = dst->ch[nch] + x;

			/* First row */
			*d = s[0]*0.6875 + s[1]*0.2500 + s[2]*0.0625;
			
			/* middle rows */
			for (unsigned y = 1; y<dheight-2;y++) {
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
	image_ptr scaled = image_alloc(dst_sizy, dst_sizx, img->nchannel);
	image_ptr tmp = image_alloc(img->sizx,dst_sizy, img->nchannel);

	/* scale in columns, and transposed */
	reduce1dtran(img,img->sizy,tmp,dst_sizy,img->sizx);
	/* scale in (old)columns, and transposed back */
	reduce1dtran(tmp,img->sizx,scaled,dst_sizx,dst_sizy);

	image_delete(tmp);
	return scaled;
}

image_ptr image_crop(const image_ptr img, fbox_t crop, int* offset, bool shared) {
	image_ptr result;
	ibox_t intcrop = fbox_getibox(&crop);
	if(shared) {
		result = new image_t;
		result->sizx = intcrop.x2-intcrop.x1+1;
		result->sizy = intcrop.y2-intcrop.y1+1;
		result->stepy = img->stepy;
		result->stepyx = img->stepyx;
		result->is_shared = true;
		result->nchannel = img->nchannel;
		result->data = img->data + (intcrop.x1*img->stepy + intcrop.y1);
		for(unsigned i=0;i<result->nchannel;i++)
			result->ch[i] = result->data + img->stepyx*i;
	} else {
		result = image_alloc(intcrop.y2-intcrop.y1+1,intcrop.x2-intcrop.x1+1,img->nchannel);
		for(unsigned c=0;c<result->nchannel;c++){
			for(unsigned y=0;y<result->sizy;y++) {
				for(unsigned x=0;x<result->sizx;x++) {
					result->data[c*result->stepyx+x*result->stepy+y] = 
						img->data[c*img->stepyx+(x+intcrop.x1)*result->sizy+(y+intcrop.y1)];
				}
			}
		}
	}
	if(offset!=NULL) {
		offset[0] = intcrop.y1;
		offset[1] = intcrop.x1;
	}
	return result;
}

void image_showDetection(const image_ptr img, const vector<bbox_t> boxes, const std::string& winname) {
	using namespace cv;
	Mat M(img->sizy,img->sizx,CV_8UC3);
	for(unsigned y=0;y<img->sizy;y++) {
		for(unsigned x=0;x<img->sizx;x++) {
			M.at<Vec3b>(y,x)[2]=img->ch[0][y+x*img->stepy];
			M.at<Vec3b>(y,x)[1]=img->ch[1][y+x*img->stepy];
			M.at<Vec3b>(y,x)[0]=img->ch[2][y+x*img->stepy];
		}
	}

	for(unsigned i=0;i<boxes.size();i++){
		int x1 = (int)boxes[i].outer.x1;
		int y1 = (int)boxes[i].outer.y1;
		int w = (int)boxes[i].outer.x2 - x1;
		int h = (int)boxes[i].outer.y2 - y1;
		rectangle(M, Rect(x1,y1,w,h),Scalar(0,255,0));
	}
	namedWindow(winname, CV_WINDOW_AUTOSIZE);
	imshow(winname,M);
	//waitKey();
}

