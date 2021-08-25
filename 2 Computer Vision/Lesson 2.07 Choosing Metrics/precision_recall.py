import numpy as np

from iou import calculate_ious
from utils import get_data


def precision_recall(ious, gt_classes, pred_classes):
    """
    calculate precision and recall
    args:
    - ious [array]: NxM array of ious
    - gt_classes [array]: 1xN array of ground truth classes
    - pred_classes [array]: 1xM array of pred classes
    returns:
    - precision [float]
    - recall [float]
    """
    gt_matched, pred_matched  = np.where(ious > 0.5)

    tp = 0
    fp = 0

    for gt, pred in zip(gt_matched, pred_matched):
        if gt_classes[gt] == pred_classes[pred]:
            tp += 1
        else:
            fp += 1

    fn = len(gt_classes) - len(np.unique(gt_matched))

    return tp / (tp + fp), tp / (fp + fn)


if __name__ == "__main__": 
    ground_truth, predictions = get_data()

    # get bboxes array
    filename = 'segment-1231623110026745648_480_000_500_000_with_camera_labels_38.png'
    gt_bboxes = [g['boxes'] for g in ground_truth if g['filename'] == filename][0]
    gt_bboxes = np.array(gt_bboxes)
    gt_classes = [g['classes'] for g in ground_truth if g['filename'] == filename][0]


    pred_bboxes = [p['boxes'] for p in predictions if p['filename'] == filename][0]
    pred_boxes = np.array(pred_bboxes)
    pred_classes = [p['classes'] for p in predictions if p['filename'] == filename][0]

    ious = calculate_ious(gt_bboxes, pred_boxes)
    precision, recall = precision_recall(ious, gt_classes, pred_classes)
    print(precision, recall)