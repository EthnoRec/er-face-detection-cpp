/*
 * eHposemodel.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHposemodel.h"
#include "eHfeatpyramid.h"
#include "eHmatrix.h"
#include "eHshiftdt.h"
#include "eHutils.h"

#include "rapidxml-1.13/rapidxml.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "iostream"
#include "fstream"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

using std::ifstream;
using std::ios;

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }

posemodel_t* posemodel_parseXml(char* xmlstr) {
	using namespace rapidxml;
	posemodel_t* model = new posemodel_t;
	xml_document<> doc;
	doc.parse<0>(xmlstr);
	xml_node<>* root = doc.first_node("posemodel");

	/* first level nodes */
	xml_node<>* biases = root->first_node("biases");
	xml_node<>* filters = root->first_node("filters");
	xml_node<>* defs = root->first_node("defs");
	xml_node<>* parts = root->first_node("parts");
	xml_node<>* maxsize = root->first_node("maxsize");
	xml_node<>* len = root->first_node("len");
	xml_node<>* interval = root->first_node("interval");
	xml_node<>* sbin = root->first_node("sbin");
	xml_node<>* thresh = root->first_node("thresh");
	xml_node<>* obj = root->first_node("obj");

	int field_width = -1;
	int* tmp_int_ptr;
	double* tmp_double_ptr;

	/* model->biases : vector<posebias_t> */
	int num_biases = strtol(biases->first_attribute("num")->value(),NULL,10);
	xml_node<>* bias = biases->first_node("bias");
	xml_node<>* bias_i = bias->first_node("i");
	xml_node<>* bias_w = bias->first_node("w");
	posebias_t tmp_posebias;
		/*1st bias*/
	tmp_posebias.i = strtol(bias_i->value(),NULL,10);
	tmp_posebias.w = strtod(bias_w->value(),NULL);
	model->biases.push_back(tmp_posebias);
		/*other biases*/
	for(int i = 1; i<num_biases; i++){
		bias = bias->next_sibling();
		bias_i = bias->first_node("i");
		bias_w = bias->first_node("w");
		tmp_posebias.i = strtol(bias_i->value(),NULL,10);
		tmp_posebias.w = strtod(bias_w->value(),NULL);
		model->biases.push_back(tmp_posebias);
	}

	/* model->filters : vector<filter_t> */
	int num_filters = strtol(filters->first_attribute("num")->value(),NULL,10);
	xml_node<>* filter = filters->first_node("filter");
	xml_node<>* filter_i = filter->first_node("i");
	xml_node<>* filter_w = filter->first_node("w");
	filter_t tmp_posefilter;
		/*1st filter*/
	tmp_posefilter.i = strtol(filter_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
	assert(field_width==3);
	tmp_posefilter.w.sizy = tmp_int_ptr[0];
	tmp_posefilter.w.sizx = tmp_int_ptr[1];
	tmp_posefilter.w.sizz = tmp_int_ptr[2];
	field_width = -1;
	tmp_posefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
	delete[] tmp_int_ptr;
	model->filters.push_back(tmp_posefilter);
		/*other filters*/
	for(int i = 1; i<num_filters; i++){
		filter = filter->next_sibling();
		filter_i = filter->first_node("i");
		filter_w = filter->first_node("w");
		tmp_posefilter.i = strtol(filter_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
		assert(field_width==3);
		tmp_posefilter.w.sizy = tmp_int_ptr[0];
		tmp_posefilter.w.sizx = tmp_int_ptr[1];
		tmp_posefilter.w.sizz = tmp_int_ptr[2];
		field_width = -1; 
		tmp_posefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
		delete[] tmp_int_ptr;
		model->filters.push_back(tmp_posefilter);
	}

	/* model->defs : vector<posedef_t> */
	int num_defs = strtol(defs->first_attribute("num")->value(),NULL,10);
	xml_node<>* def = defs->first_node("def");
	xml_node<>* def_i = def->first_node("i");
	xml_node<>* def_w = def->first_node("w");
	xml_node<>* def_anchor = def->first_node("anchor");
	posedef_t tmp_posedef;
		/*1st def*/
	tmp_posedef.i = strtol(def_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
	assert(field_width==1);assert(tmp_int_ptr[0]==4);
	field_width = -1;
	tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]);
	for(int ii=0;ii<field_width;ii++) tmp_posedef.w[ii]=tmp_double_ptr[ii];
	delete[] tmp_int_ptr;
	delete[] tmp_double_ptr;
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
	assert(field_width==3);
	tmp_posedef.anchor[0]=tmp_int_ptr[0]-1;
	tmp_posedef.anchor[1]=tmp_int_ptr[1]-1;
	tmp_posedef.anchor[2]=tmp_int_ptr[2];
	delete[] tmp_int_ptr;
	model->defs.push_back(tmp_posedef);
		/*other defs*/
	for(int i=1;i<num_defs;i++){
		def = def->next_sibling();
		def_i = def->first_node("i");
		def_w = def->first_node("w");
		def_anchor = def->first_node("anchor");
		tmp_posedef.i = strtol(def_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
		assert(field_width==1);
		field_width = -1;
		tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]);
		for(int ii=0;ii<field_width;ii++) tmp_posedef.w[ii]=tmp_double_ptr[ii];
		delete[] tmp_int_ptr;
		delete[] tmp_double_ptr;
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
		assert(field_width==3);
		tmp_posedef.anchor[0]=tmp_int_ptr[0]-1;
		tmp_posedef.anchor[1]=tmp_int_ptr[1]-1;
		tmp_posedef.anchor[2]=tmp_int_ptr[2];
		delete[] tmp_int_ptr;
		model->defs.push_back(tmp_posedef);
	}

	/* model->parts : vector<posepart_t> */
	int num_parts = strtol(parts->first_attribute("num")->value(),NULL,10);
	xml_node<>* part;
	posepart_t tmp_posepart;
		/*1st part*/
	part = parts->first_node("part");
	tmp_posepart.num = strtol(part->first_attribute("num")->value(),NULL,10);
	tmp_posepart.numpar = strtol(part->first_attribute("numpar")->value(),NULL,10);
	tmp_posepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
	tmp_posepart.biasid = new int;
	tmp_posepart.biasid[0] = strtol(part->first_attribute("biasid")->value(),NULL,10)-1;
	tmp_posepart.defid = NULL; /*root has no defid*/
	field_width = -1;
	tmp_posepart.filterid = parseCSVstr2int(part->first_attribute("filterid")->value(),&field_width);
	assert(tmp_posepart.num == field_width);
	model->parts.push_back(tmp_posepart);
		/*other parts*/
	for(int i =1;i<num_parts; i++) {
		part = part->next_sibling();
		tmp_posepart.num = strtol(part->first_attribute("num")->value(),NULL,10);
		tmp_posepart.numpar = strtol(part->first_attribute("numpar")->value(),NULL,10);
		tmp_posepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		field_width = -1;
		tmp_posepart.biasid = parseCSVstr2int(part->first_attribute("biasid")->value(),&field_width);
		assert(tmp_posepart.num*tmp_posepart.numpar == field_width);
		field_width = -1;
		tmp_posepart.defid = parseCSVstr2int(part->first_attribute("defid")->value(),&field_width);
		assert(tmp_posepart.num == field_width);
		field_width = -1;
		tmp_posepart.filterid = parseCSVstr2int(part->first_attribute("filterid")->value(),&field_width);
		assert(tmp_posepart.num == field_width);
		model->parts.push_back(tmp_posepart);
	}

	/* model->maxsize : int[2] */
	field_width = -1;
	tmp_int_ptr=parseCSVstr2int(maxsize->first_attribute("size")->value(),&field_width);
	assert(field_width==2);
	model->maxsize[0]=tmp_int_ptr[0];
	model->maxsize[1]=tmp_int_ptr[1];
	delete[] tmp_int_ptr;

	/* model->len : int */
	model->len = strtol(len->first_attribute("val")->value(),NULL,10);

	/* model->interval : int */
	model->interval = strtol(interval->first_attribute("val")->value(),NULL,10);

	/* model->sbin : int */
	model->sbin = strtol(sbin->first_attribute("val")->value(),NULL,10);

	/* model->thresh : double */
	model->thresh = strtod(thresh->first_attribute("val")->value(),NULL);

	/* model->obj : double */
	model->obj = strtod(obj->first_attribute("val")->value(),NULL);

	/* additional fields (not from file) :
	 * sizy, sizx, starty, startx, scale, step
	 * an assumption seems hold: all ds are the same, so only 1 scale and step 
	 * stored for each part*/
	model->parts[0].sizy=model->parts[0].sizx=NULL;
	model->parts[0].starty=model->parts[0].startx=NULL;
	model->parts[0].scale=0;model->parts[0].step=1; /*XXX useful?*/
	posepart_t* part_ptr;
	int num, par, ds, step, virtpady, virtpadx;
	for (unsigned i=1;i<model->parts.size();i++){
		part_ptr = &(model->parts[i]);
		par = part_ptr->parent;
		assert(par<(int)i);
		num = part_ptr->num;
		part_ptr->sizy = new int[num];
		part_ptr->sizx = new int[num];
		part_ptr->starty = new int[num];
		part_ptr->startx = new int[num];
		ds = model->defs[part_ptr->defid[0]].anchor[2];
		step = (ds==0 ? 1 : int(pow(2.0,(double)ds)));
		virtpady = (step-1)*max(model->maxsize[0]-1-1,0);
		virtpadx = (step-1)*max(model->maxsize[1]-1-1,0);
		part_ptr->step = step;
		part_ptr->scale = ds + model->parts[par].scale;
		for(int j=0;j<part_ptr->num;j++) {
			part_ptr->sizy[j] = model->filters[part_ptr->filterid[j]].w.sizy;
			part_ptr->sizx[j] = model->filters[part_ptr->filterid[j]].w.sizx;
			part_ptr->starty[j] = model->defs[part_ptr->defid[j]].anchor[1]-virtpady;
			part_ptr->startx[j] = model->defs[part_ptr->defid[j]].anchor[0]-virtpadx;
		}
	}
	return model;
}

posemodel_t* posemodel_readFromFile(const char* filepath) {
	ifstream fin(filepath);
	if (fin.fail()) return NULL;
	fin.seekg(0,ios::end);
	size_t length = fin.tellg();
	char* xmlstr = new char[length+1];
	fin.seekg(0,ios::beg);
	fin.read(xmlstr,length);
	xmlstr[length] = '\0';
	fin.close();
	posemodel_t* model = posemodel_parseXml(xmlstr);
	delete[] xmlstr;
	return model;
}

vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img, double thrs){
	vector<bbox_t> boxes;
	int imsize[] = {img->sizy, img->sizx};
	featpyra_t* pyra = featpyra_create(img, model->interval, model->sbin, model->maxsize, false);
	
	/*TODO */
	return boxes;
}
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img) {
	return posemodel_detect(model, img, model->thresh);
}

void posemodel_delete(posemodel_t* model) {
	/*
	 * filters[i].w.vals was allocated inside parseCSV2double
	 */
	for(unsigned i=0;i<model->filters.size();i++)
		delete[] model->filters[i].w.vals;
	/*
	 * parts[i].biasid/defid/filterid were allocated inside parseCSV2int
	 * parts[i].sizy/sizx/starty/startx were newed
	 */
	delete model->parts[0].biasid;
	delete[] model->parts[0].filterid;
	for(unsigned i=1;i<model->parts.size();i++) {
		delete[] model->parts[i].biasid;
		delete[] model->parts[i].defid;
		delete[] model->parts[i].filterid;
		delete[] model->parts[i].sizy;
		delete[] model->parts[i].sizx;
		delete[] model->parts[i].starty;
		delete[] model->parts[i].startx;
	}	
	delete model;
}
