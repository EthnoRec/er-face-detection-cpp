#!/usr/bin/python3

import os
import glob
import subprocess
import optparse

def find_faces(imgDir, facemodelFile, posemodelFile, saveImgDir): 
    imgDir = os.path.expanduser(imgDir)
    saveImgDir = os.path.expanduser(saveImgDir)
    if not os.path.exists(saveImgDir): 
        os.makedirs(saveImgDir)
    imgs = glob.glob(os.path.join(imgDir,"*.jpg"))
    for i,img in enumerate(imgs):
        print("Processing {0}/{1}: {2}".format(i,len(imgs),img))
        subprocess.call(["./facefinder",img,facemodelFile,posemodelFile,os.path.join(saveImgDir,os.path.basename(img))])
        
def main():
    usage = "usage: %prog [options] IMAGE_DIR SAVE_DIR"
    parser = optparse.OptionParser(usage)
    parser.add_option("-f","--facemodel",dest="facemodel",metavar="FACE_MODEL")
    parser.add_option("-p","--posemodel",dest="posemodel",metavar="POSE_MODEL",default="-")
    (options,args) = parser.parse_args()
    find_faces(args[0],options.facemodel,options.posemodel,args[1])

if __name__ == '__main__': 
    main()
