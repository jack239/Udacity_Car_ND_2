import copy
import json
from collections import Counter

import matplotlib.pyplot as plt
import numpy as np

from utils import calculate_iou, check_results

def have_isec(box, boxes, min_iou = 0.5):
    for box2 in boxes:
        if calculate_iou(box, box2) > min_iou:
            return True
    return False

def get_pr_curve(predictions, boxes):
    tp = 0
    pr_curve = []
    for pred in predictions:
        if have_isec(pred[1], boxes) and pred[0] == 1:
            tp += 1
            updated = 1
        else:
            updated = 0
        pr_curve.append([
            tp / (len(pr_curve) + 1.),
            tp / len(boxes), 
            updated
        ])
    return np.array(pr_curve)

def get_smoothed(pr_curve):
    zero_pos = [i for i, p in enumerate(pr_curve) if p[2] == 0]
    smoothed = copy.copy(pr_curve[:,:2])
    for i in range(len(zero_pos)):
        start = zero_pos[i]
        if i + 1 < len(zero_pos):
            end = zero_pos[i + 1] 
        else:
            end = len(pr_curve)
        smoothed[start:end,0] = max(pr_curve[start:end,0])        
    return smoothed

def get_mAP(smoothed):
    cmin = 0
    mAP = 0
    for i in range(smoothed.shape[0] - 1):
        if smoothed[i, 1] == smoothed[i+1, 1]:
            mAP += (smoothed[i, 1] - cmin) * smoothed[i, 0]
            cmin = smoothed[i, 1]
    mAP += (smoothed[-1, 1] - cmin) * smoothed[-1, 0]
    return mAP

if __name__ == '__main__':
    # load data 
    with open('data/predictions.json', 'r') as f:
        preds = json.load(f)[0]

    with open('data/ground_truths.json', 'r') as f:
        gts = json.load(f)[0]
    
    predictions = list(zip(preds["classes"], preds["boxes"], preds["scores"]))
    predictions.sort(key = lambda x: -x[-1])
    # TODO IMPLEMENT THIS SCRIPT
    pr_curve = get_pr_curve(predictions, gts["boxes"])
    smoothed = get_smoothed(pr_curve)
    
    plt.plot(pr_curve[:, 1], pr_curve[:, 0], linewidth=4)
    plt.plot(smoothed[:, 1], smoothed[:, 0], linewidth=4)
    plt.xlabel('recall', fontsize=18)
    plt.ylabel('precision', fontsize=18)
    plt.show()
    mAP = get_mAP(smoothed)
    check_results(mAP)