import tensorflow as tf

from solution.utils import check_softmax, check_acc, check_model, check_ce

def softmax(logits):
    exp = tf.exp(logits)
    exp_sum = tf.math.reduce_sum(exp, 1, keepdims=True)
    return exp / exp_sum


def cross_entropy(scaled_logits, one_hot):
    bool_logits = tf.boolean_mask(scaled_logits, one_hot) 
    return -tf.math.log(bool_logits)


def model(X, W, b):
    flatten_X = tf.reshape(X, (-1, W.shape[0]))
    return softmax(tf.matmul(flatten_X, W) + b)


def accuracy(y_hat, Y):
    y_pred = tf.cast(tf.argmax(y_hat, axis=1), Y.dtype)
    return tf.math.reduce_sum(tf.cast(y_pred == Y, tf.int32)) / Y.shape[0]

if __name__ == '__main__':
    # checking the softmax implementation
    check_softmax(softmax)

    # checking the NLL implementation
    check_ce(cross_entropy)

    # check the model implementation
    check_model(model)

    # check the accuracy implementation
    check_acc(accuracy)
