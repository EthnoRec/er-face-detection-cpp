/*
 * eHimageFeature.h
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHimage.h"
#include "eHmatrix.h"

/* Compute HOG feature of a color image
 * usage: 
 * 	mat3d_ptr F = eHhog(image_ptr img,int sbin)
 * 	img - 3 channel double precision image
 * 	sbin - bin size
 *
 * NOTE:
 * 	(1)all matrices/images are in column-major style (as in Fortran & Matlab)
 * 	(2)mat3d_ptr F is allocated inside, proper delete is necessary after use
 */
mat3d_ptr eHhog(const image_ptr img, int sbin);
