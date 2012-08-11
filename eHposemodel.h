/*
 * eHposemodel.h
 *
 * Data structures used to describe and store 
 * human body/pose model information
 *
 * Hang Su
 * 2012-08 @ eH
 */
#ifndef EHPOSEMODEL_H
#define EHPOSEMODEL_H

#include <vector>
#include "eHimage.h"
#include "eHfilter.h"
#include "eHbbox.h"

using std::vector;

typedef struct deformation_pose {
	int i;
	int anchor[3];
	double w[4];
} posedef_t;

typedef struct part_pose {
	int* biasid; /*numpar x num (except for root, which is 1)*/
	int* defid; /*num (except for root, which is NULL)*/
	int* filterid; /*num*/
	int num;
	int numpar;
	int parent;
	/* infos not directly from file */
	int* sizy; /*num*/
	int* sizx;/*num*/
	int scale;
	int step;
	int* startx;/*num*/
	int* starty;/*num*/
} posepart_t;

typedef struct bias_pose {
	int i;
	double w;
} posebias_t;

typedef struct eHposemodel {
	vector<posebias_t> biases;
	vector<filter_t> filters;
	vector<posedef_t> defs;
	vector<posepart_t> parts;
	int maxsize[2];
	int len;
	int interval;
	int sbin;
	double thresh;
	double obj;
} posemodel_t;

/*
 * Parse pose model from xml style string
 * NOTE: xmlstr is modified during parsing, this can be avoided by 
 * using Non-Destrutive Mode of rapidxml
 */
posemodel_t* posemodel_parseXml(char* xmlstr);

/*
 * Read pose model from file
 */
posemodel_t* posemodel_readFromFile(const char* filepath);

/*
 * Detection
 * First one take thrs as an input, second use the value in model
 */
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img, double thrs);
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img);

/*
 * Delete a pose model, release related memory
 */
void posemodel_delete(posemodel_t* model);


#endif
