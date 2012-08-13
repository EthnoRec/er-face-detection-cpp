/*
 * eHutils.h
 * Some useful stuff (string parsing etc. )
 *
 * Hang Su
 * 2012-08 @ eH
 */
#ifndef EHUTILS_H
#define EHUTILS_H

/*
 * parse given string(e.g. "10, 5\0") to integer array
 *
 * NOTE: 
 * 	(1)integer array is allocated inside
 * 	(2)siz==-1 indicates size is not known, will be decided automatically
 * 	(3)if siz is wrong, behavior is undefined
 */
int* parseCSVstr2int(const char* csvstr, int* siz, int offset = 0);

/*
 * parse given string(e.g. "1.2, 3.4\0") to double precision array
 *
 * NOTE: 
 * 	(1)integer array is allocated inside
 * 	(2)siz==-1 indicates size is not known, will be decided automatically
 * 	(3)if siz is wrong, behavior is undefined
 */
double* parseCSVstr2double(const char* csvstr, int* siz);

#endif
