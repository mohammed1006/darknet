import os
from shutil import copyfile


lines = [line.strip() for line in open("/home/ubuntu/DATA/darknet/darknet_ori/datasets/train.txt")]

dst = "/home/ubuntu/DATA/darknet/data_folder"

if not os.path.exists(dst):
    os.makedirs(dst)

if not os.path.exists(dst + "/JPEGImages"):
    os.makedirs(dst + "/JPEGImages")

if not os.path.exists(dst + "/labels"):
    os.makedirs(dst + "/labels")

for line in lines:
    # os.system('echo ' + ' cp ' + line + ' ' + dst + "/JPEGImages/")
    os.system('cp ' + line + ' ' + dst + "/JPEGImages/")
    label = line.replace("images", "labels").replace("JPEGImages", "labels").replace(".jpg", ".txt").replace(".JPEG", ".txt")
    # os.system('echo ' + ' cp ' + label + ' ' + dst + "/labels/")
    os.system('cp ' + label + ' ' + dst + "/labels/")
