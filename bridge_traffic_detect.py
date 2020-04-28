from ctypes import *
import math
import random
import os
import cv2
import numpy as np
import time
import darknet
from shapely.geometry import Polygon, Point
from estimate_distance import EstimatorOfDistance


def convertBack(x, y, w, h):
    xmin = int(round(x - (w / 2)))
    xmax = int(round(x + (w / 2)))
    ymin = int(round(y - (h / 2)))
    ymax = int(round(y + (h / 2)))
    return xmin, ymin, xmax, ymax


def cvDrawBoxes_v2(detections, img, color):
    for detection in detections:
        x, y, w, h = detection[2][0],\
            detection[2][1],\
            detection[2][2],\
            detection[2][3]
        xmin, ymin, xmax, ymax = convertBack(
            float(x), float(y), float(w), float(h))
        pt1 = (xmin, ymin)
        pt2 = (xmax, ymax)
        cv2.rectangle(img, pt1, pt2, color, 1)
        cv2.putText(img,
                    detection[0].decode() +
                    " [" + str(round(detection[1] * 100, 2)) + "]",
                    (pt1[0], pt1[1] - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                    color, 2)
    return img


def cvDrawBoxes(detections, img):
    for detection in detections:
        x, y, w, h = detection[2][0],\
            detection[2][1],\
            detection[2][2],\
            detection[2][3]
        xmin, ymin, xmax, ymax = convertBack(
            float(x), float(y), float(w), float(h))
        pt1 = (xmin, ymin)
        pt2 = (xmax, ymax)
        cv2.rectangle(img, pt1, pt2, (0, 255, 0), 1)
        cv2.putText(img,
                    detection[0].decode() +
                    " [" + str(round(detection[1] * 100, 2)) + "]",
                    (pt1[0], pt1[1] - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
                    [0, 255, 0], 2)
    return img


netMain = None
metaMain = None
altNames = None


def video_detect():

    global metaMain, netMain, altNames
    configPath = "./cfg/yolov4.cfg"
    weightPath = "./yolov4.weights"
    metaPath = "./cfg/coco.data"
    VideoPath = "D:/data/bridge/yinhe_bridge_south.mp4"

    estim = EstimatorOfDistance()

    # Define road range
    point_zero = Point(0, 607)
    left_lane_border_w_offset = [Point(0, 493), Point(168, 418), Point(286, 368), Point(348, 345), Point(405, 326)]
    mid_lane_w_offset = [Point(0, 582), Point(225, 438), Point(326, 381), Point(380, 354), Point(421, 333)]
    right_lane_border_w_offset = []
    point_cnt = len(estim.markers)
    for i in range(point_cnt):
        right_lane_border_w_offset.append(Point(estim.markers[i]))
    road_points = [mid_lane_w_offset[-1]]
    road_points.extend(right_lane_border_w_offset[::-1])
    road_points.append(point_zero)
    road_points.extend(left_lane_border_w_offset)

    road = Polygon(road_points)

    # Define lane, line01 = left lane in the video. The remaining area is lane02 (will not define here).

    line01_points = mid_lane_w_offset[::-1]
    line01_points.extend(left_lane_border_w_offset)

    line01 = Polygon(line01_points)

    if not os.path.exists(configPath):
        raise ValueError("Invalid config path `" +
                         os.path.abspath(configPath)+"`")
    if not os.path.exists(weightPath):
        raise ValueError("Invalid weight path `" +
                         os.path.abspath(weightPath)+"`")
    if not os.path.exists(metaPath):
        raise ValueError("Invalid data file path `" +
                         os.path.abspath(metaPath)+"`")
    if netMain is None:
        netMain = darknet.load_net_custom(configPath.encode(
            "ascii"), weightPath.encode("ascii"), 0, 1)  # batch size = 1
    if metaMain is None:
        metaMain = darknet.load_meta(metaPath.encode("ascii"))
    if altNames is None:
        try:
            with open(metaPath) as metaFH:
                metaContents = metaFH.read()
                import re
                match = re.search("names *= *(.*)$", metaContents,
                                  re.IGNORECASE | re.MULTILINE)
                if match:
                    result = match.group(1)
                else:
                    result = None
                try:
                    if os.path.exists(result):
                        with open(result) as namesFH:
                            namesList = namesFH.read().strip().split("\n")
                            altNames = [x.strip() for x in namesList]
                except TypeError:
                    pass
        except Exception:
            pass
    #cap = cv2.VideoCapture(0)
    cap = cv2.VideoCapture(VideoPath)
    cap.set(3, 1280)
    cap.set(4, 720)
    # out = cv2.VideoWriter(
    #     "output.avi", cv2.VideoWriter_fourcc(*"MJPG"), 10.0,
    #     (darknet.network_width(netMain), darknet.network_height(netMain)))
    print("Starting the YOLO loop...")

    # Create an image we reuse for each detect
    darknet_image = darknet.make_image(darknet.network_width(netMain),
                                    darknet.network_height(netMain),3)
    while True:
        prev_time = time.time()
        ret, frame_read = cap.read()
        if frame_read is not None:
            frame_rgb = cv2.cvtColor(frame_read, cv2.COLOR_BGR2RGB)
            frame_resized = cv2.resize(frame_rgb,
                                       (darknet.network_width(netMain),
                                        darknet.network_height(netMain)),
                                       interpolation=cv2.INTER_LINEAR)

            darknet.copy_image_from_bytes(darknet_image, frame_resized.tobytes())

            detections = darknet.detect_image(netMain, metaMain, darknet_image, thresh=0.25)
            # Parse detections results
            det_cars = []
            line01_cars = []
            line02_cars = []
            total_class_names = []
            for det in detections:
                class_name, conf, loc = det
                class_name = class_name.decode()
                if class_name not in total_class_names:
                    total_class_names.append(class_name)
                if class_name == 'car':
                    x, y, w, h = loc
                    if road.contains(Point(x, y)):
                        # in-line check
                        print(x, y)
                        if line01.contains(Point(x, y)):
                            line01_cars.append(det)
                        else:
                            line02_cars.append(det)

                        det_cars.append(det)
                        cv2.circle(frame_resized, (int(x), int(y)), 2, (255, 0, 0), 0)

            # line01_x = []
            # line02_x = []
            # if len(line01_cars) > 1:
            #     for det in line01_cars:
            #         class_name, conf, loc = det
            #         x, y, w, h = loc
            #         line01_x.append(x)
            #     min_x = min(line01_x)
            #     min_idx = line01.index(min_x)
            #     for i in range(len(line01_cars)):
            #         if i != min_idx:
            #             # calculate distance between the nearest car and current car
            #             pass

            # image = cvDrawBoxes(det_cars, frame_resized)
            image = cvDrawBoxes_v2(line01_cars, frame_resized, (255, 0, 0))
            image = cvDrawBoxes_v2(line02_cars, image, (0, 255, 0))
            image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
            print('fps', 1/(time.time() - prev_time))
            cv2.imshow('Demo', image)
            cv2.waitKey(3)
        else:
            print('Empty frame')
            break

    cap.release()
    # out.release()


if __name__ == "__main__":
    video_detect()
