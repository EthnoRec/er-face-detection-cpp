#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"

#include "string.h"

#include "dirent.h"

#include <iostream>
#include <vector>
#include <sys/time.h>

#define millisecs(x) ((x).tv_sec*1000 + (x).tv_usec/1000)

using std::cout;
using std::endl;
using std::vector;

#ifdef EH_TEST_TIMER
extern struct timeval time_spent_dp;
extern struct timeval time_spent_conv;
extern struct timeval time_spent_pyra;
extern struct timeval time_spent_detect;
#endif

int main(int argc, char** argv){
	
	struct timeval time_spent_total, total_start, total_end;
	struct timeval time_spent_local, local_start, local_end;
	gettimeofday(&total_start, NULL);
	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	posemodel_t* posemodel = posemodel_readFromFile("pose_BUFFY.xml");
/*
	char path[200];
	if(argc<2)
		strcpy(path,"/home/hang/ppoc10k/photo_female_5k/");
	else
		strcpy(path,argv[1]);
	DIR* dir;
	struct dirent *ent;
	char name[200];
	dir = opendir(path);
	int cnt=0;
	if(dir!=NULL) {
		while((ent=readdir(dir))!=NULL) {
			if(strstr(ent->d_name,".jpg")) {
				cnt++;
				strcpy(name, path);
				strcat(name, ent->d_name);
				cout<<name<<" ...";
				image_t* img = image_readJPG(name);
				gettimeofday(&local_start,NULL);
				vector<bbox_t> faces = facemodel_detect(model, posemodel, img);
				gettimeofday(&local_end,NULL);
				timersub(&local_end,&local_start,&time_spent_local);
				timersub(&local_end,&total_start,&time_spent_total);
				cout<<millisecs(time_spent_local)<<" ms, avg="<<millisecs(time_spent_total)/(double)cnt<<" ms"<<endl;
				image_showDetection(img, faces, "result");
				image_delete(img);

			}
		}
	}
	gettimeofday(&total_end, NULL);
	timersub(&total_end, &total_start, &time_spent_total);
	cout<<cnt<<" photo processed"<<endl;
	cout<<"time_spent_avg: "<<millisecs(time_spent_total)/(double)cnt<<" ms"<<endl;
	
*/	
	image_t* img = NULL;
	if(argc<2)
		//img = image_readJPG("image/10156670_4.jpg");
		img = image_readJPG("hard/hard_face.jpg");
	else
		img = image_readJPG(argv[1]);
	image_display(img,"test");
	vector<bbox_t> faces = facemodel_detect(model,posemodel,img);
	image_showDetection(img, faces, "results");
	
	//vector<bbox_t> poses = posemodel_detect(posemodel,img);
	
	image_delete(img);
#ifdef EH_TEST_TIMER
	cout<<"time_spent_detect: "<<millisecs(time_spent_detect)<<" ms"<<endl
		<<" - time_spent_pyra: "<<millisecs(time_spent_pyra)<<" ms"<<endl
		<<" - time_spent_conv: "<<millisecs(time_spent_conv)<<" ms"<<endl
		<<" - time_spent_dp: "<<millisecs(time_spent_dp)<<" ms"<<endl;
#endif

	facemodel_delete(model);
	posemodel_delete(posemodel);
	return 0;
}
