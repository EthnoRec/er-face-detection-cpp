/*
 * eHfacemodel.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfacemodel.h"
#include "eHutils.h"
#include "rapidxml-1.13/rapidxml.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

using std::vector;
using std::ifstream;
using std::ios;

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }

/* NOTE: change field_width to actual value (and remove assertion) 
 * can get higher speed
 *
 * index changed from matlab 0-1 style to C style:
 *  - def.anchor[0] XXX 
 *  - def.anchor[1] XXX
 *  - part.defid 
 *  - part.filterid
 *  - part.parent 
 * index NOT changed from matlab style:
 *  - filter.i XXX
 *  - def.i XXX
 */
facemodel_t* facemodel_parseXml(char* xmlstr) {
	using namespace rapidxml;
	facemodel_t* model = new facemodel_t;
	xml_document<> doc;
	doc.parse<0>(xmlstr);
	xml_node<>* root = doc.first_node();

	/* first level nodes */
	xml_node<>* filters = root->first_node("filters");
	xml_node<>* defs = root->first_node("defs");
	xml_node<>* components = root->first_node("components");
	xml_node<>* maxsize = root->first_node("maxsize");
	xml_node<>* len = root->first_node("len");
	xml_node<>* interval = root->first_node("interval");
	xml_node<>* sbin = root->first_node("sbin");
	xml_node<>* delta = root->first_node("delta");
	xml_node<>* thresh = root->first_node("thresh");
	xml_node<>* obj = root->first_node("obj");

	int field_width=-1;
	int* tmp_int_ptr;	
	double* tmp_double_ptr;

	/* model->filters : vector<facefilter_t> */
	int num_filters = strtol(filters->first_attribute("num")->value(),NULL,10);
	xml_node<>* filter = filters->first_node("filter");
	xml_node<>* filter_i = filter->first_node("i");
	xml_node<>* filter_w = filter->first_node("w");
	facefilter_t tmp_facefilter;
		/*1st filter*/
	tmp_facefilter.i = strtol(filter_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
	assert(field_width==3);
	tmp_facefilter.w.sizy = tmp_int_ptr[0];
	tmp_facefilter.w.sizx = tmp_int_ptr[1];
	tmp_facefilter.w.sizz = tmp_int_ptr[2];
	field_width = -1;
	tmp_facefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
	delete[] tmp_int_ptr;
	model->filters.push_back(tmp_facefilter);
		/*other filters*/
	for(int i = 1; i<num_filters; i++){
		filter = filter->next_sibling();
		filter_i = filter->first_node("i");
		filter_w = filter->first_node("w");
		tmp_facefilter.i = strtol(filter_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
		assert(field_width==3);
		tmp_facefilter.w.sizy = tmp_int_ptr[0];
		tmp_facefilter.w.sizx = tmp_int_ptr[1];
		tmp_facefilter.w.sizz = tmp_int_ptr[2];
		field_width = -1; 
		tmp_facefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
		delete[] tmp_int_ptr;
		model->filters.push_back(tmp_facefilter);
	}

	/* model->defs : vector<facedef_t> */
	int num_defs = strtol(defs->first_attribute("num")->value(),NULL,10);
	xml_node<>* def = defs->first_node("def");
	xml_node<>* def_i = def->first_node("i");
	xml_node<>* def_w = def->first_node("w");
	xml_node<>* def_anchor = def->first_node("anchor");
	facedef_t tmp_facedef;
		/*1st def*/
	tmp_facedef.i = strtol(def_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
	assert(field_width==1);
	field_width = -1;
	tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]);
	for(int ii=0;ii<field_width;ii++) tmp_facedef.w[ii]=tmp_double_ptr[ii];
	delete[] tmp_int_ptr;
	delete[] tmp_double_ptr;
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
	assert(field_width==3);
	tmp_facedef.anchor[0]=tmp_int_ptr[0]-1;
	tmp_facedef.anchor[1]=tmp_int_ptr[1]-1;
	tmp_facedef.anchor[2]=tmp_int_ptr[2];
	delete[] tmp_int_ptr;
	model->defs.push_back(tmp_facedef);
		/*other defs*/
	for(int i=1;i<num_defs;i++){
		def = def->next_sibling();
		def_i = def->first_node("i");
		def_w = def->first_node("w");
		def_anchor = def->first_node("anchor");
		tmp_facedef.i = strtol(def_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
		assert(field_width==1);
		field_width = -1;
		tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]);
		for(int ii=0;ii<field_width;ii++) tmp_facedef.w[ii]=tmp_double_ptr[ii];
		delete[] tmp_int_ptr;
		delete[] tmp_double_ptr;
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
		assert(field_width==3);
		tmp_facedef.anchor[0]=tmp_int_ptr[0]-1;
		tmp_facedef.anchor[1]=tmp_int_ptr[1]-1;
		tmp_facedef.anchor[2]=tmp_int_ptr[2];
		delete[] tmp_int_ptr;
		model->defs.push_back(tmp_facedef);
	}

	/* model->components : vector<vector<facepart_t> >*/
	int num_components = strtol(components->first_attribute("num")->value(),NULL,10);
	int num_parts;
	xml_node<>* component = components->first_node("component");
	xml_node<>* part;
	facepart_t tmp_facepart;
	vector<facepart_t> tmp_vector_facepart;
		/*1st component*/
	num_parts = strtol(component->first_attribute("num")->value(),NULL,10);
	part = component->first_node("part");
	tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
	tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
	tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
	tmp_vector_facepart.clear();
	tmp_vector_facepart.push_back(tmp_facepart);
	for(int j=1;j<num_parts;j++){
		part = part->next_sibling();
		tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
		tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
		tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		tmp_vector_facepart.push_back(tmp_facepart);
	}
	model->components.push_back(tmp_vector_facepart);
		/*other components*/
	for(int i =1;i<num_components; i++) {
		component = component->next_sibling();
		num_parts = strtol(component->first_attribute("num")->value(),NULL,10);
		part = component->first_node("part");
		tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
		tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
		tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		tmp_vector_facepart.clear();
		tmp_vector_facepart.push_back(tmp_facepart);
		for(int j=1;j<num_parts;j++){
			part = part->next_sibling();
			tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
			tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
			tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
			tmp_vector_facepart.push_back(tmp_facepart);
		}
		model->components.push_back(tmp_vector_facepart);

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

	/* model->delta : double */
	model->delta = strtod(delta->first_attribute("val")->value(),NULL);

	/* model->thresh : double */
	model->thresh = strtod(thresh->first_attribute("val")->value(),NULL);

	/* model->obj : double */
	model->obj = strtod(obj->first_attribute("val")->value(),NULL);

	/* additional fields (not from file) */
	facepart_t* part_ptr;
	int par, ax, ay, ds, step, virtpady, virtpadx;
	for (int i=0;i<model->components.size();i++){
		for (int j=0; j<model->components[i].size();j++) {
			part_ptr = &(model->components[i][j]);
			part_ptr->sizy = model->filters[part_ptr->filterid].w.sizy;
			part_ptr->sizx = model->filters[part_ptr->filterid].w.sizx;
			/* store the offset and scale of each part relative to the root */
			ax = model->defs[part_ptr->defid].anchor[0];
			ay = model->defs[part_ptr->defid].anchor[1];
			ds = model->defs[part_ptr->defid].anchor[2];
			if ((par = part_ptr->parent)>=0) {
				part_ptr->scale = ds + model->components[i][par].scale;
			} else {
				assert(j==0);
				part_ptr->scale = 0;
			}
			assert(par<j);
			/* amount of (virtual) padding to hallucinate */
			if (ds==0) step = 1;else
				step = int(pow(2.0,(double)ds)+0.1);
			virtpady = (step-1)*max(model->maxsize[0]-1-1,0);
			virtpadx = (step-1)*max(model->maxsize[1]-1-1,0);
			/* starting points (simulates additional padding at finer scales */
			part_ptr->starty = ay-virtpady;
			part_ptr->startx = ax-virtpadx;
			part_ptr->step = step;
		}
	}

	return model;
}

facemodel_t* facemodel_readFromFile(const char* filepath) {
	ifstream fin(filepath);
	if (fin.fail()) return NULL;
	fin.seekg(0,ios::end);
	size_t length = fin.tellg();
	char* xmlstr = new char[length+1];
	fin.seekg(0,ios::beg);
	fin.read(xmlstr,length);
	xmlstr[length] = '\0';
	fin.close();
	facemodel_t* model = facemodel_parseXml(xmlstr);
	delete[] xmlstr;
	return model;
}

void facemodel_delete(facemodel_t* model) {
	/* because filters[i].w.vals was allocated inside parseCSV2double, 
	 * it should be released manually */
	for(int i=0;i<model->filters.size();i++)
		delete[] model->filters[i].w.vals;
	delete model;
}
