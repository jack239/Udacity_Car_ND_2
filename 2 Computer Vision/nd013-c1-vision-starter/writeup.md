# Data
## Download tfrecords
In first step I modified `download_process.py` to ignore files which were downloaded earlier.

## Exploratory Data Analysis and Split Creation
### Display 10 images
![ten_images](result/ten_frames.png)

### Create the splits
Create cross validation split from  download tfrecords. 
To save disk space I created soft link in 'create_splits.py'.

## Training
### Create config for network
#### First attempt
For successfully first run there config file required some modification:
* To avoid OOM batch_size decreased to 2. 
* `metrics_set` changed to `pascal_voc_detection_metrics`. Link to 

![first](result/first_training.png)
```
PascalBoxes_Precision/mAP@0.5IOU: 0.017973
PascalBoxes_PerformanceByCategory/AP@0.5IOU/vehicle: 0.035946
PascalBoxes_PerformanceByCategory/AP@0.5IOU/pedestrian: 0.000000
PascalBoxes_PerformanceByCategory/AP@0.5IOU/cyclist: nan
Loss/localization_loss: 0.505468
Loss/classification_loss: 0.479383
Loss/regularization_loss: 1.848156
Loss/total_loss: 2.833003
```