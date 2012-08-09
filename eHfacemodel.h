/*
 * eHfacemodel.h
 *
 * Data structures used to describe and 
 * store model information
 *
 * Hang Su
 * 2012-08 @ eH
 */
#ifndef EHFACEMODEL_H
#define EHFACEMODEL_H

#include <vector>
#include "eHmatrix.h"
#include "eHimage.h"
#include "eHfilter.h"
#include "eHbbox.h"

#define EH_TEST_TIMER

using std::vector;

typedef struct deformation {
	int i;
	int anchor[3]; /*ax ay ds*/
	double w[4]; /*only 1st is used for root*/
} facedef_t;

typedef struct part {
	int defid;
	int filterid;
	int parent;
	/*infos not directly from file*/
	int sizy;
	int sizx;
	int scale;
	int startx;
	int starty;
	int step;
	/*cache for local usage, NOT part of model*/
	double* score;
	int sizScore[2];
	int* Ix;
	int* Iy;
	int sizI[2];/*Ix, Iy, normally the same as sizScore*/
	int level;
	

} facepart_t;

/* XXX assume all filters are of the same size
 * (e.g. face_p146.xml), otherwise code need further 
 * modification
 */
typedef struct eHfacemodel {
	vector<filter_t> filters;
	vector<facedef_t> defs;
	vector<vector<facepart_t> > components;
	int maxsize[2];
	int len;
	int interval;
	int sbin;
	double delta;
	double thresh;
	double obj;
} facemodel_t;

/*
 * Parse face model from xml style string
 * NOTE: xmlstr is modified during parsing, this can be avoided by 
 * using Non-Destrutive Mode of rapidxml
 */
facemodel_t* facemodel_parseXml(char* xmlstr);

/*
 * Read face model from file
 */
facemodel_t* facemodel_readFromFile(const char* filepath);

/*
 * Detection
 */
vector<bbox_t> facemodel_detect(const image_ptr img, facemodel_t* model, double thrs = -0.65);

/*
 * Delete a face model, release related memory
 */
void facemodel_delete(facemodel_t* model);

#endif
