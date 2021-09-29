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
Before training config file is necessary. 
