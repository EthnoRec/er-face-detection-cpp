#include "eHimage.h"
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHbbox.h"

#include "string.h"

#include "dirent.h"

#include <iostream>
#include <iomanip>
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
	//Step 1: load face model
	facemodel_t* model = facemodel_readFromFile("face_p146.xml");
	//Step 2: load body model
	posemodel_t* posemodel = posemodel_readFromFile("pose_BUFFY.xml");

	struct timeval time_spent_total;
	struct timeval time_spent_local, local_start, local_end;
	time_spent_total.tv_sec = time_spent_total.tv_usec = 0;

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
				cout<<std::setw(20)<<name<<" ...";
				//Step 3: read image
				image_t* img = image_readJPG(name);
				gettimeofday(&local_start,NULL);
				//Step 4: detect faces
				//vector<bbox_t> faces = facemodel_detect(model, posemodel, img);
				vector<bbox_t> faces = facemodel_detect(model, img);
				gettimeofday(&local_end,NULL);
				timersub(&local_end,&local_start,&time_spent_local);
				timeradd(&time_spent_local,&time_spent_total,&time_spent_total);
				cout<<std::setw(8)<<millisecs(time_spent_local)<<" ms, avg="<<std::setw(8)
					<<millisecs(time_spent_total)/(double)cnt<<" ms"<<endl;
				//Step 5: show results
				image_showDetection(img, faces, "result");
				//Step 6a: delete image
				image_delete(img);

			}
		}
	}
	cout<<cnt<<" photo processed"<<endl;
	cout<<"time_spent_avg: "<<millisecs(time_spent_total)/(double)cnt<<" ms"<<endl;

/*	
	image_t* img = NULL;
	if(argc<2)
		img = image_readJPG("image/10156670_4.jpg");
	else
		img = image_readJPG(argv[1]);
	vector<bbox_t> faces = facemodel_detect(model,posemodel,img);
	image_showDetection(img, faces, "result");
	image_showFaces(img, faces, "result");
	
	image_delete(img);
#ifdef EH_TEST_TIMER
	cout<<"time_spent_detect: "<<millisecs(time_spent_detect)<<" ms"<<endl
		<<" - time_spent_pyra: "<<millisecs(time_spent_pyra)<<" ms"<<endl
		<<" - time_spent_conv: "<<millisecs(time_spent_conv)<<" ms"<<endl
		<<" - time_spent_dp: "<<millisecs(time_spent_dp)<<" ms"<<endl;
#endif
*/
	//Step 6b: delete face model
	facemodel_delete(model);
	//Step 6c: delete body model
	posemodel_delete(posemodel);
	return 0;
}
