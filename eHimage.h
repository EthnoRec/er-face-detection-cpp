/*
 * eHimage.h
 * Basic image type and operations
 *
 * Hang Su
 * 2012-07 @ eH
 */
#ifndef EHIMAGE_H
#define EHIMAGE_H

#include <stdlib.h>
#include <math.h>
#include <string>

/* Basic image data structure
 * default rgb : ch[0]=r ch[1]=g ch[2]=b
 * Using column major (Fortran) style
 */
typedef struct eHimage {
	double* data;
	double* ch[3];
	size_t sizy;
	size_t sizx;
	size_t nchannel;
} image_t;
typedef image_t* image_ptr;

/*
 * Allocation of a new image of size [sizy, sizx]
 * Returned image is not zeroed
 */
image_ptr image_alloc(size_t sizy, size_t sizx, size_t nch = 3);

/*
 * Delete image and associated memory
 * Note: a new image is returned, old one is not deleted
 */
void image_delete(image_ptr);

/*
 * Read Jpeg image file
 */
image_ptr image_readJPG(const char* filename);

/*
 * Display an image
 */
void image_display(const image_ptr img, const std::string& winname);
	
/*
 * Resize image to given scale
 */
image_ptr image_resize(const image_ptr, double scale);

/*
 * Return an image half the size of given image
 * Note: a new image is returned, old one is not deleted
 */
image_ptr image_reduce(const image_ptr);

#endif
