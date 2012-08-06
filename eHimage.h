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

/* Basic color image data structure
 * default 3 channels: ch[0]=b ch[1]=g ch[2]=r
 * pixel values encoded in vectors, with column major (Fortran) style
 */
typedef struct eHimage {
	double* ch[3];
	size_t sizy;
	size_t sizx;
} image_t;
typedef image_t* image_ptr;

/*
 * Allocation of a new image of size [sizy, sizx]
 * Returned image is not zeroed
 */
image_ptr image_alloc(size_t sizy, size_t sizx);

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
 * Resize image to given scale
 */
image_ptr image_resize(const image_ptr, double scale);

/*
 * Return an image half the size of given image
 * Note: a new image is returned, old one is not deleted
 */
image_ptr image_reduce(const image_ptr);

#endif
