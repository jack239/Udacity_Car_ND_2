#!/bin/bash
#run docker
docker run -it --shm-size 2000000000 -v ${PWD}:/app/project/ project-dev bash

#load data files
curl https://sdk.cloud.google.com | bash
export PATH=$PATH:/root/google-cloud-sdk/bin/
gcloud auth login
python download_process.py --data_dir data_waymo --temp_dir tmp_data

#split data
export DATA_DIR=data_waymo/processed/
python create_splits.py --data_dir ${DATA_DIR}

#create config
export PRETRAIN_DIN=pretrained-models/ssd_resnet50_v1_fpn_640x640_coco17_tpu-8
python edit_config.py --train_dir ${DATA_DIR}/train/ --eval_dir ${DATA_DIR}/val/ --batch_size 4 --checkpoint ${PRETRAIN_DIN}/checkpoint/ckpt-0 --label_map label_map.pbtxt
export TRAINING_DIR=training/reference/
mkdir -p $TRAINING_DIR
cp pipeline_new.config  $TRAINING_DIR

#train
python experiments/model_main_tf2.py --model_dir=${TRAINING_DIR} --pipeline_config_path=${TRAINING_DIR}pipeline_new.config
python experiments/model_main_tf2.py --model_dir=${TRAINING_DIR} --pipeline_config_path=${TRAINING_DIR}pipeline_new.config --checkpoint_dir=${TRAINING_DIR}


#process data
python experiments\exporter_main_v2.py --input_type image_tensor --pipeline_config_path training/experiment0/pipeline.config --trained_checkpoint_dir training/experiment0/ckpt-50 --output_directory training/experiment0/exported_model/
python inference_video.py -labelmap_path label_map.pbtxt --model_path training/experiment0/exported_model/saved_model --tf_record_path ${DATA_DIR}/test/tf.record --config_path training/experiment0/pipeline_new.config --output_path animation.mp4

#jupyter notebook
jupyter notebook --allow-root --ip=0.0.0.0

pip install tensorflow-gpu
pip install nvidia-cudnn

os.environ["CUDA_VISIBLE_DEVICES"]
nvidia-pyindex
nvidia-cudnn
