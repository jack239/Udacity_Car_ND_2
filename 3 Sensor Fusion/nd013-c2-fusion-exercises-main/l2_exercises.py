# ---------------------------------------------------------------------
# Exercises from lesson 2 (object detection)
# Copyright (C) 2020, Dr. Antje Muntzinger / Dr. Andreas Haja.  
#
# Purpose of this file : Starter Code
#
# You should have received a copy of the Udacity license together with this program.
#
# https://www.udacity.com/course/self-driving-car-engineer-nanodegree--nd013
# ----------------------------------------------------------------------
#

from PIL import Image
import io
import sys
import os
import cv2
import open3d as o3d
import math
import numpy as np
import zlib

import matplotlib
# matplotlib.use('wxagg') # change backend so that figure maximizing works on Mac as well
import matplotlib.pyplot as plt

# Exercise C2-4-6 : Plotting the precision-recall curve
def plot_precision_recall(): 

    # Please note: this function assumes that you have pre-computed the precions/recall value pairs from the test sequence
    #              by subsequently setting the variable configs.conf_thresh to the values 0.1 ... 0.9 and noted down the results.
    
    # Please create a 2d scatter plot of all precision/recall pairs 
    pass


# Exercise C2-3-4 : Compute precision and recall
def compute_precision_recall(det_performance_all, conf_thresh=0.5):
    if len(det_performance_all) == 0:
        print("no detections for conf_thresh = " + str(conf_thresh))
        return
    pos_negs = np.sum([item[-1] for item in det_performance_all], axis=0)
    positives, true_positives, false_positives, false_negatives = pos_negs

    # extract the total number of positives, true positives, false negatives and false positives
    # format of det_performance_all is [ious, center_devs, pos_negs]

    print(f"TP = {true_positives}, FP = {false_positives}, FN = {false_negatives}")

    # compute precision
    precision = true_positives / positives

    # compute recall
    recall = true_positives / (true_positives + false_negatives)

    print(f"precision = {precision}, recall = {recall}, conf_thres = {conf_thresh}\n")


# Exercise C2-3-2 : Transform metric point coordinates to BEV space
def pcl_to_bev(lidar_pcl, configs, vis=True):
    lidar_pcl = lidar_pcl.copy()
    def convert(arr, lim, target):
        return (arr - lim[0]) / (lim[1] - lim[0]) * target


    # compute bev-map discretization by dividing x-range by the bev-image height
    # create a copy of the lidar pcl and transform all metrix x-coordinates into bev-image coordinates
    # transform all metrix y-coordinates as well but center the foward-facing x-axis on the middle of the image
    lidar_pcl[lidar_pcl[:, 3] > 1.0, 3] = 1.0
    lidar_pcl[:, 0] = convert(lidar_pcl[:, 0], configs.lim_x, configs.bev_height - 1).astype(int)
    lidar_pcl[:, 1] = convert(lidar_pcl[:, 1], configs.lim_y, configs.bev_width - 1).astype(int)
    lidar_pcl[:, 2] = convert(lidar_pcl[:, 2], configs.lim_z, 1)
    intensity_lim = [np.amin(lidar_pcl[:, 3]), np.amax(lidar_pcl[:, 3])]
    lidar_pcl[:, 3] = convert(lidar_pcl[:, 3], intensity_lim, 1)

    def get_map(index):
        order = np.lexsort([-lidar_pcl[:, index], lidar_pcl[:, 0], lidar_pcl[:, 1]])
        sorted_lidar_pcl = lidar_pcl[order]
        unique_order = np.unique(sorted_lidar_pcl[:,:2], axis=0, return_index=True)[1]
        sorted_lidar_pcl = sorted_lidar_pcl[unique_order]
        result_map = np.zeros((configs.bev_height, configs.bev_width))
        result_map[sorted_lidar_pcl[:,0].astype(int), sorted_lidar_pcl[:,1].astype(int)] = sorted_lidar_pcl[:, index]
        return result_map
    height_map = get_map(2)
    intensity_map = get_map(3)
    # shift level of ground plane to avoid flipping from 0 to 255 for neighboring pixels

    # re-arrange elements in lidar_pcl_cpy by sorting first by x, then y, then by decreasing height

    # extract all points with identical x and y such that only the top-most z-coordinate is kept (use numpy.unique)

    # assign the height value of each unique entry in lidar_top_pcl to the height map and
    # make sure that each entry is normalized on the difference between the upper and lower height defined in the config file

    # sort points such that in case of identical BEV grid coordinates, the points in each grid cell are arranged based on their intensity

    # only keep one point per grid cell

    # create the intensity map

    # visualize intensity map
    if vis:
       img_intensity = intensity_map * 256
       img_intensity = img_intensity.astype(np.uint8)
       while (1):
           cv2.imshow('img_intensity', img_intensity)
           if cv2.waitKey(10) & 0xFF == 27:
               break
       cv2.destroyAllWindows()
    pass
