from darknet import performDetect
from glob import glob
import os

files = glob('pothole_weights/data/*.jpg')
OUT_DIR = './pothole_weights/out'
for file in files:
    base_name = os.path.basename(file)
    out_path = os.path.join(OUT_DIR, base_name)
    result = performDetect(file, 0.3, "pothole_weights/pothole-yolov3-tiny.cfg", "pothole_weights/pothole-yolov3-tiny.weights", "./cfg/obj.data", saveImage=out_path)
