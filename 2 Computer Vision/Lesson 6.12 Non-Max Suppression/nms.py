import json

from utils import calculate_iou, check_results


def nms(predictions):
    boxes = predictions["boxes"]
    scores = predictions["scores"]
    skipped = [False] * len(boxes)
    for i in range(len(boxes) - 1):
        for j in range(i + 1, len(boxes)):
            if calculate_iou(boxes[i], boxes[j]) > 0.5:
                if scores[i] > scores[j]:
                    skipped[j] = True
                else:
                    skipped[i] = True
    result = [[box, score] for (box, score, skip) in zip(boxes, scores, skipped) if not skip]
    return result

if __name__ == '__main__':
    with open('data/predictions_nms.json', 'r') as f:
        predictions = json.load(f)
    
    filtered = nms(predictions)
    check_results(filtered)