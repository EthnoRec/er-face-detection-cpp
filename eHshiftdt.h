/*
 * eHshiftdt.h
 *
 * Generalized Distance Transform
 * see: "Distance Transforms of Sampled Functions" 
 * (P. F. Felzenszwalb and D. P. Huttenlocher, 2004)
 * This applies computes a min convolution of a quadratic function ax^2+bx
 *
 * Hang Su
 * 2012-07 @ eH
 */

/*
 * NOTE: M, Ix, Iy should already be allocated before passed in, 
 * they are then modified as output results
 * M, Ix, Iy, vals - column first order
 */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		const double* vals, int sizx, int sizy, 
		const double* w);

/* wrapper for default setting */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		const double* vals, int sizx, int sizy, 
		const double* w);
