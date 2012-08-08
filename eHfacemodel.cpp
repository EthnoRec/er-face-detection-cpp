/*
 * eHfacemodel.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfacemodel.h"
#include "eHfeatpyramid.h"
#include "eHutils.h"

#include "rapidxml-1.13/rapidxml.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"

#include <vector>
#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

#define EH_MAX_LEN 800

#define EH_PH_TESTING

using std::vector;
using std::ifstream;
using std::ios;

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }

static double msg_cache[EH_MAX_LEN*EH_MAX_LEN];

void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		double* vals, int sizx, int sizy, 
		double* w);
mat3d_ptr eHconv(const mat3d_ptr feats, const vector<facefilter_t> filters, int start, int end);
void eHnms(vector<bbox_t>& bboxes, double overlap);

timeval  time_spent_dgemv;
timeval time_spent_conv;
timeval  time_spent_pyra;
timeval  time_spent_feat;
timeval  time_spent_dp;

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
				step = int(pow(2.0,(double)ds));
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

vector<bbox_t> facemodel_detect(const image_ptr img, facemodel_t* model, double thrs) {
	vector<bbox_t> boxes;

#ifdef 	EH_PH_TESTING
	time_spent_dgemv.tv_sec=time_spent_dgemv.tv_usec=0;
	time_spent_conv.tv_sec=time_spent_conv.tv_usec=0;;
	time_spent_pyra.tv_sec=time_spent_pyra.tv_usec=0;
	time_spent_dp.tv_sec=time_spent_dp.tv_usec=0;
	time_spent_feat.tv_sec=time_spent_feat.tv_usec=0;
#endif

	/* build feature pyramid */
	model->interval = 5;
	int imsize[] = {img->sizy, img->sizx};

	timeval start_pyra, end_pyra, interval_pyra;
	gettimeofday(&start_pyra,NULL);
	facepyra_t* pyra = facepyra_create(img, model->interval, model->sbin, model->maxsize);
	gettimeofday(&end_pyra,NULL);
	timersub(&end_pyra,&start_pyra,&interval_pyra);
	timeradd(&interval_pyra,&time_spent_pyra, &time_spent_pyra);

	int minlevel = model->interval+1;
	mat3d_ptr* resp = new mat3d_ptr[pyra->len];
	memset(resp, 0, pyra->len*sizeof(mat3d_ptr));

 	/* index changed from matlab 0-1 style to C style:
	 * 	level
	 */
	int c, rlevel, k; /* component#, root level, part# */
	for(c=0;c<model->components.size();c++){
		vector<facepart_t>* parts = &(model->components[c]);
		int numparts = parts->size();
		for(rlevel=model->interval;rlevel<pyra->len;rlevel++){
			facepart_t* rootpart = &(parts->at(0));

			/* local part scores */
			for(k=0;k<numparts;k++){
				int fid = parts->at(k).filterid;
				int level = rlevel-(parts->at(k)).scale*model->interval;
				if(resp[level]==NULL){
					timeval start_conv, end_conv, interval_conv;
					gettimeofday(&start_conv,NULL);
					resp[level]=eHconv(pyra->feat[level], model->filters, 0, model->filters.size()-1);
					gettimeofday(&end_conv,NULL);
					timersub(&end_conv,&start_conv,&interval_conv);
					timeradd(&interval_conv,&time_spent_conv,&time_spent_conv);
				}
				parts->at(k).level = level;
				int len = (resp[level]->sizy)*(resp[level]->sizx);
				parts->at(k).score = new double[len];
				memcpy(parts->at(k).score,resp[level]->vals+fid*len,len*sizeof(double));
				parts->at(k).sizScore[0] = resp[level]->sizy;
				parts->at(k).sizScore[1] = resp[level]->sizx;
			}

			timeval start_dp, end_dp, interval_dp;
			gettimeofday(&start_dp,NULL);
			/* part relations - tree message passing */
			for(k=numparts-1;k>0;k--){
				facepart_t* child = &(parts->at(k));
				int par = child->parent;
				int Ny = parts->at(par).sizScore[0];
				int Nx = parts->at(par).sizScore[1];
				/* assume all filters are of the same size */
				assert(Ny == child->sizScore[0] && Nx == child->sizScore[1]);
				double* msg;
				if(Nx*Ny>EH_MAX_LEN*EH_MAX_LEN)
					msg = new double[Nx*Ny];
				else
					msg = msg_cache;
				child->Iy = new int[Ny*Nx];
				child->Ix = new int[Ny*Nx];
				eHshiftdt(msg,child->Ix,child->Iy,Nx,Ny,
					child->startx,child->starty,
					child->step,child->score,
					child->sizScore[1],child->sizScore[0],
					model->defs[child->defid].w
					);
				for(int i=0;i<Ny*Nx;i++)
					parts->at(par).score[i]+=msg[i];
				if(Nx*Ny>EH_MAX_LEN*EH_MAX_LEN)
					delete[] msg;
			}
			
			/* add bias to root score (const term) */
			vector<int> slct; slct.reserve(10000);
			for(int i=0;i<rootpart->sizScore[0]*rootpart->sizScore[1];i++) {
				rootpart->score[i] += model->defs[rootpart->defid].w[0];
				if(rootpart->score[i]>=thrs)
					slct.push_back(i);
			}
			
			if(!slct.empty()) {
			/* backtrack */
				/* root */
				int k0=boxes.size();
				int newboxes_len = slct.size();
				boxes.resize(k0+newboxes_len);
				int* ptr = new int[numparts*newboxes_len];/*XXX*/
				int padx = max(model->maxsize[1]-2,0);
				int pady = max(model->maxsize[0]-2,0);
				double scale;
				for(int i=0;i<newboxes_len;i++){
					ptr[i]=slct[i];
					scale = pyra->scale[rootpart->level];
					int y = slct[i]%rootpart->sizScore[0];
					int x = (slct[i]-y)/rootpart->sizScore[0];
					fbox_t tmpbox = {
						(x-padx)*scale,
						(y-pady)*scale,
						(x-padx+rootpart->sizx)*scale-1,
						(y-pady+rootpart->sizy)*scale-1
					};
					boxes[k0+i].boxes.push_back(tmpbox);
					boxes[k0+i].component = c;
					boxes[k0+i].score = rootpart->score[slct[i]];
				}
				/*remaining parts*/
				for(k=1;k<numparts;k++){
					facepart_t* tmppart = &(parts->at(k));
					int par = tmppart->parent;
					scale = pyra->scale[tmppart->level];
					int x,y;
					for(int i=0;i<newboxes_len;i++){
						x = tmppart->Ix[ptr[par*newboxes_len+i]];
						y = tmppart->Iy[ptr[par*newboxes_len+i]];
						ptr[k*newboxes_len+i] = x*tmppart->sizScore[0]+y;
						fbox_t tmpbox = {
							(x-padx)*scale,
							(y-pady)*scale,
							(x-padx+tmppart->sizx)*scale-1,
							(y-pady+tmppart->sizy)*scale-1
						};
						boxes[k0+i].boxes.push_back(tmpbox);
					}
				}

				delete[] ptr;
			}
			gettimeofday(&end_dp,NULL);
			timersub(&end_dp,&start_dp,&interval_dp);
			timeradd(&interval_dp, &time_spent_dp, &time_spent_dp);

			/* clean Ix Iy score */
			for(k=numparts-1;k>0;k--){
				facepart_t* child = &(parts->at(k));
				delete[] child->Ix;
				delete[] child->Iy;
				delete[] child->score;
			}

		}
	}

	for(int i=0;i<pyra->len;i++)
		if(resp[i]!=NULL) mat3d_delete(resp[i]);
	delete[] resp;
	facepyra_delete(pyra);

	for (int i=0; i<boxes.size(); i++)
		bbox_clipboxes(boxes[i],imsize);
	eHnms(boxes, 0.3);

	/*testing code: display outer bbox*/
	using namespace cv;
	Mat M(img->sizy,img->sizx,CV_8UC3);
	for(int y=0;y<img->sizy;y++) {
		for(int x=0;x<img->sizx;x++) {
			M.at<Vec3b>(y,x)[0]=img->ch[0][y+x*img->sizy];
			M.at<Vec3b>(y,x)[1]=img->ch[1][y+x*img->sizy];
			M.at<Vec3b>(y,x)[2]=img->ch[2][y+x*img->sizy];
		}
	}
	for(int i=0;i<boxes.size();i++){
		int x1 = (int)boxes[i].outer.x1;
		int y1 = (int)boxes[i].outer.y1;
		int w = (int)boxes[i].outer.x2 - x1;
		int h = (int)boxes[i].outer.y2 - y1;
		rectangle(M, Rect(x1,y1,w,h),Scalar(0,255,0));
	}
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	imshow("test",M);
	waitKey();

	return boxes;
}

void facemodel_delete(facemodel_t* model) {
	/* because filters[i].w.vals was allocated inside parseCSV2double, 
	 * it should be released manually */
	for(int i=0;i<model->filters.size();i++)
		delete[] model->filters[i].w.vals;
	delete model;
}
