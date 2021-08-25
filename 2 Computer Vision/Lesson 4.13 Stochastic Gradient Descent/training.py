import argparse
import logging
import tensorflow as tf

from dataset import get_datasets
from logistic import softmax, cross_entropy, accuracy


def get_module_logger(mod_name):
    logger = logging.getLogger(mod_name)
    handler = logging.StreamHandler()
    formatter = logging.Formatter('%(asctime)s %(levelname)-8s %(message)s')
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)
    return logger


def sgd(params, grads, learn_rate, batch_size):
    """
    stochastic gradient descent implementation
    args:
    - params [list[tensor]]: model params
    - grad [list[tensor]]: param gradient such that params[0].shape == grad[0].shape
    - lr [float]: learning rate
    - bs [int]: batch_size
    """
    for param, grad in zip(params, grads):
        param.assign_sub(grad * learn_rate / batch_size)

def model(X):
    flatten_X = tf.reshape(X, (-1, W.shape[0]))
    return softmax(tf.matmul(flatten_X, W) + b)

def get_prediction(X, model):
    return model(X / 255.0)

def train_on_data(tape, X, Y, model, loss_function, optimizer, lern_rate=0.1):
    # IMPLEMENT THIS FUNCTION
    y_pred = get_prediction(X, model)
    one_hot = tf.one_hot(Y, 43)

    loss = loss_function(y_pred, one_hot)

    grads = tape.gradient(loss, [W, b])
    optimizer([W, b], grads, lern_rate, X.shape[0])

    acc = accuracy(y_pred, Y)

    return loss, acc

def training_loop(dataset, model, loss_function, optimizer):
    """
    training loop
    args:
    - train_dataset: 
    - model [func]: model function
    - loss [func]: loss function
    - optimizer [func]: optimizer func
    returns:
    - mean_loss [tensor]: mean training loss
    - mean_acc [tensor]: mean training accuracy
    """
    accuracies = []
    losses = []
    for X, Y in dataset:
        with tf.GradientTape() as tape:
            accuracie, loss = train_on_data(tape, X, Y, model, loss_function, optimizer)
            accuracies.append(accuracie)
            losses.append(loss)
            pass
    mean_acc = tf.math.reduce_mean(tf.concat(accuracies, axis=0))
    mean_loss = tf.math.reduce_mean(losses)
    return mean_loss, mean_acc


def validation_loop(dataset, model):
    accuracies = []
    for X, Y in dataset:
        with tf.GradientTape() as tape:
            y_pred = get_prediction(X, model)
            accuracie = accuracy(y_pred, Y)
            accuracies.append(accuracie)
            pass
    mean_acc = tf.math.reduce_mean(tf.concat(accuracies, axis=0))
    return mean_acc


if __name__  == '__main__':
    logger = get_module_logger(__name__)
    parser = argparse.ArgumentParser(description='Download and process tf files')
    parser.add_argument('--imdir', required=True, type=str,
                        help='data directory')
    parser.add_argument('--epochs', default=10, type=int,
                        help='Number of epochs')
    args = parser.parse_args()    

    logger.info(f'Training for {args.epochs} epochs using {args.imdir} data')
    # get the datasets
    train_dataset, val_dataset = get_datasets(args.imdir)

    # set the variables
    num_inputs = 1024*3
    num_outputs = 43
    W = tf.Variable(tf.random.normal(shape=(num_inputs, num_outputs),
                                    mean=0, stddev=0.01))
    b = tf.Variable(tf.zeros(num_outputs))

    # training! 
    for epoch in range(args.epochs):
        logger.info(f'Epoch {epoch}')
        loss, acc = training_loop(train_dataset, model, cross_entropy, sgd)
        logger.info(f'Mean training loss: {loss}, mean training accuracy {acc}')
        acc = validation_loop(val_dataset, model)
        logger.info(f'Mean validation accuracy {acc}')
