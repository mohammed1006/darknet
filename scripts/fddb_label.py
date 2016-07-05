import pickle
import os
from os import listdir, getcwd
from os.path import join
import imghdr
from PIL import Image

def ensureFolderForFile(output):
    print "ensureFolderForFile " + output
    folderName = output[0: output.rfind("/")]
    if not os.path.exists(folderName):
        os.makedirs(folderName)

DATA_FOLDER = "/home/ubuntu/DATA/Datasets/FDDB/originalPics"
ANNO_FOLDER = "/home/ubuntu/DATA/Datasets/FDDB/FDDB-folds"

JPEG_FOLDER = "/home/ubuntu/DATA/Datasets/FDDB/JPEGImages"
LABELS_FOLDER = "/home/ubuntu/DATA/Datasets/FDDB/labels"
TRAIN_TXT_FILE = "/home/ubuntu/DATA/Datasets/FDDB/train.txt"

if not os.path.exists(JPEG_FOLDER):
    os.makedirs(JPEG_FOLDER)

if not os.path.exists(LABELS_FOLDER):
    os.makedirs(LABELS_FOLDER)

trainFile = open(TRAIN_TXT_FILE, "w")

annoListText = os.listdir(ANNO_FOLDER)

annoFiles = []
for text in annoListText:
    if "ellipseList" in text:
        annoFiles.append(text)

PATH_STATE = 1
COUNT_STATE = 2
ANNO_STATE = 3

for annoFile in annoFiles:
    lines = [line.strip() for line in open(ANNO_FOLDER + "/" + annoFile)]

    curState = PATH_STATE
    curPath = ""
    outputPath = ""
    curLabel = None
    curCount = 0
    imgWidth = 0
    imgHeight = 0
    # for i in range(0, len(lines)):
        # print lines[i]
    for line in lines:
        if curState == PATH_STATE:
            curPath = DATA_FOLDER + "/" + line + ".jpg"
            outputPath = JPEG_FOLDER + "/" + line + ".jpg"
            curState = COUNT_STATE
            print "line = " + line

            #ensure folder exist
            ensureFolderForFile(outputPath)
            imgWidth = 0
            imgHeight = 0
            if imghdr.what(curPath) != None:
                try:
                    im = Image.open(curPath)
                    imgWidth, imgHeight = im.size
                except Exception as e:
                    print e
                    raise

                os.system('cp ' + curPath + ' ' + outputPath)
                trainFile.write(outputPath + "\n")
            else:
                print "invalid file jpeg"
        elif curState == COUNT_STATE:
            curCount = int(line)
            print "count = " + line
            if curCount > 0:
                curLabelPath = outputPath.replace("JPEGImages", "labels").replace(".jpg", ".txt")
                #ensure folder exist
                ensureFolderForFile(curLabelPath)
                curLabel = open(curLabelPath, "w")
            curState = ANNO_STATE
        elif curState == ANNO_STATE:
            print "anno = " + line
            if imgWidth > 0:
                annoData = line.replace("  ", " ").split(" ")
                major_axis = float(annoData[0] ) * 2/ imgHeight
                minor_axis = float(annoData[1] ) * 2/ imgWidth

                center_x = float(annoData[3]) / imgWidth
                center_y = float(annoData[4]) / imgHeight

                curLabel.write("0 " + str(center_x) + " " + str(center_y) + " " + str(minor_axis) + " " + str(major_axis) + "\n")
            curCount = curCount - 1
            if curCount <= 0:
                curState = PATH_STATE
