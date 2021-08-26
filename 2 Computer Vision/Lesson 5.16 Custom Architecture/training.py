import argparse
import logging

import tensorflow as tf
from tensorflow.keras.layers import Dense, Flatten, Conv2D, MaxPooling2D, InputLayer

from utils import get_datasets, get_module_logger, display_metrics

def get_flatten_layer():
    return [
        Flatten,
        {}
    ]

def get_full_layer(size):
    return [
        Dense,
        {
            "units" : size,
            "activation": "relu"
        }
    ]

def get_solution_layers():
    max_pool_layer = [
        MaxPooling2D,
        {
            "pool_size": (2, 2),
            "strides": (2, 2),
            "padding": "valid",
        }
    ]
    layers = [
        [
            Conv2D,
            {
                "filters" :6,
                "kernel_size" : (3, 3),
                "strides" : (1, 1),
                "activation" :"relu",
            }
        ],
        max_pool_layer,
        [
            Conv2D,
            {
                "filters": 16,
                "kernel_size": (3, 3),
                "strides": (1, 1),
                "activation": "relu",
            }
        ],
        max_pool_layer,
        get_flatten_layer(),
        get_full_layer(120),
        get_full_layer(84),
    ]
    return layers

def get_AlexNet_layers():
    max_pool_layer = [
        MaxPooling2D,
        {
            "pool_size": (2, 2),
            "strides": (3, 3),
            "padding": "valid",
        }
    ]
    layers = [
        [
            Conv2D,
            {
                "filters" :96,
                "kernel_size" : (11, 11),
                "strides" : (1, 1),
                "padding" : "same",
                "activation" :"relu",
            }
        ],
        max_pool_layer,
        [
            Conv2D,
            {
                "filters": 256,
                "kernel_size": (11, 11),
                "strides": (1, 1),
                "padding" : "same",
                "activation": "relu",
            }
        ],
        max_pool_layer,
        get_flatten_layer(),
        get_full_layer(120),
        get_full_layer(84),
    ]
    return layers

def create_network():
    net = tf.keras.models.Sequential()
    input_shape = [32, 32, 3]
    net.add(InputLayer(input_shape=input_shape))
    for layer, params in get_AlexNet_layers():
        net.add(layer(**params))

    net.add(Dense(43))
    return net

if __name__  == "__main__":
    logger = get_module_logger(__name__)
    parser = argparse.ArgumentParser(description="Download and process tf files")
    parser.add_argument("-d", "--imdir", required=True, type=str,
                        help="data directory")
    parser.add_argument("-e", "--epochs", default=10, type=int,
                        help="Number of epochs")
    args = parser.parse_args()    

    logger.info(f"Training for {args.epochs} epochs using {args.imdir} data")
    # get the datasets
    train_dataset, val_dataset = get_datasets(args.imdir)

    model = create_network()

    model.compile(optimizer="adam",
              loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
              metrics=["accuracy"])
    history = model.fit(x=train_dataset, 
                        epochs=args.epochs, 
                        validation_data=val_dataset)
    display_metrics(history)