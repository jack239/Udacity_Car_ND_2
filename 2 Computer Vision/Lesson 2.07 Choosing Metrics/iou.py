import numpy as np

from utils import get_data, check_results


def calculate_ious(gt_bboxes, pred_bboxes):
    """
    calculate ious between 2 sets of bboxes 
    args:
    - gt_bboxes [array]: Nx4 ground truth array
    - pred_bboxes [array]: Mx4 pred array
    returns:
    - iou [array]: NxM array of ious
    """
    ious = np.zeros((gt_bboxes.shape[0], pred_bboxes.shape[0]))
    for i, gt_bbox in enumerate(gt_bboxes):
        for j, pred_bbox in enumerate(pred_bboxes):
            ious[i,j] = calculate_iou(gt_bbox, pred_bbox)
    return ious

def get_intersec(p1, p2, q1, q2):
    start = max(q1, p1)
    end = min (q2, p2)
    return max(0, end - start)

def get_s(bbox):
    return (bbox[2] - bbox[0]) * (bbox[3] - bbox[1])

def calculate_iou(gt_bbox, pred_bbox):
    dx = get_intersec(gt_bbox[0], gt_bbox[2], pred_bbox[0], pred_bbox[2])
    dy = get_intersec(gt_bbox[1], gt_bbox[3], pred_bbox[1], pred_bbox[3])
    if dx == 0 or dy == 0:
        return 0
    s = dx * dy
    gt_s = get_s(gt_bbox)
    pred_s = get_s(pred_bbox)
    return s / (gt_s + pred_s - s)


if __name__ == "__main__": 
    ground_truth, predictions = get_data()
    # get bboxes array
    filename = 'segment-1231623110026745648_480_000_500_000_with_camera_labels_38.png'
    gt_bboxes = [g['boxes'] for g in ground_truth if g['filename'] == filename][0]
    gt_bboxes = np.array(gt_bboxes)
    pred_bboxes = [p['boxes'] for p in predictions if p['filename'] == filename][0]
    pred_boxes = np.array(pred_bboxes)
    
    ious = calculate_ious(gt_bboxes, pred_boxes)
    check_results(ious)