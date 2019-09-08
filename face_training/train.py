import tensorflow as tf
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import cv2
import os
from sklearn.model_selection import train_test_split
#os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
def read_image(path, images, labels, num_image, n):
    i = 0
    for image in os.listdir(path):
        if 'png' in image or 'jpg' in image:
            filename = path+'/'+image
            img = cv2.imread(filename, 0)
            #print(filename, img)
            #print(np.shape(img))
            img = np.ravel(img)
            #print(filename, img)
            #print(np.shape(img))
            images.append(img)
            print(image)
            if n == 'Haotian': l= [1,0,0]
            elif n== 'Gaojing': l = [0,1,0]
            else: l = [0,0,1]
            print(l)
            labels.append(l)
            i += 1
            if(i == num_image):
                break

def weight_variable(shape):
    initial = tf.truncated_normal(shape, stddev = 0.1)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.1, shape = shape)
    return tf.Variable(initial)

def conv2d(x, W):
    # stride [1, x_movement, y_movement, 1]
    return tf.nn.conv2d(x, W, strides=[1,1,1,1], padding = 'SAME')

def max_pool_2x2(x):
    # stride [1, x_movement, y_movement, 1]
    return tf.nn.max_pool(x, ksize = [1,2,2,1], strides = [1,2,2,1], padding='SAME')

def compute_accuracy(v_xs, v_ys, sess, prediction):
    y_pre = sess.run(prediction, feed_dict = {xs: v_xs, keep_prob: 1})
    correct_prediction = tf.equal(tf.argmax(y_pre, 1), tf.argmax(v_ys,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    result = sess.run(accuracy, feed_dict={xs: v_xs, ys: v_ys, keep_prob: 1})
    return result

def construct_neuron_layer(x_image, keep_prob):
    # Conv Layer 1
    W_conv1 = weight_variable([2, 2, 1,32])                  # kernel 3x3, grey scale image 1 layer, out size 32
    b_conv1 = bias_variable([32])
    h_conv1 = tf.nn.relu(conv2d(x_image, W_conv1) + b_conv1) # output size = 100x100x32
    h_pool1 = max_pool_2x2(h_conv1)                          # output size = 50x50x32
    
    # Conv Layer 2
    W_conv2 = weight_variable([2,2,32,64])                   # kernel 3x3, in size(layers) 32, out size 64
    b_conv2 = bias_variable([64])
    h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2) + b_conv2) # output size 50x50x64
    h_pool2 = max_pool_2x2(h_conv2)                          # output size 25x25x64
    
    # Fully Connected Layer 1
    W_fc1 = weight_variable([25*25*64, 1024])
    b_fc1 = bias_variable([1024])
    h_pool2_flat = tf.reshape(h_pool2, [-1, 25*25*64])       # flat the pooling output, -1 can be used to infer the shape 
    h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, W_fc1)+ b_fc1)
    h_fc1_drop = tf.nn.dropout(h_fc1, rate = 1-keep_prob)
    
    # Fully Connected Layer 2
    W_fc2 = weight_variable([1024, 3])
    b_fc2 = bias_variable([3])
    logits = tf.add(tf.matmul(h_fc1_drop, W_fc2),b_fc2)
    return logits

    

def train(images, labels, split_rate, learning_rate, num_epoch, batch_size):

    
    images = np.array(images)
    labele = np.array(labels)
    train_Data, test_Data, train_label, test_label = train_test_split(images, labels, test_size=split_rate)
    
    logits = construct_neuron_layer(x_image, keep_prob)
    prediction = tf.nn.softmax(logits)
    loss = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=logits, labels=ys))
    train_step = tf.train.AdamOptimizer(learning_rate).minimize(loss)

    saver = tf.train.Saver()
    init = tf.global_variables_initializer()

    num_batch = len(train_Data)//batch_size
    
    with tf.Session() as sess:
        sess.run(init)
        for i in range(num_epoch):
            for j in range(num_batch):
                batch_Data = train_Data[j*batch_size: (j+1)*batch_size]
                batch_label = train_label[j*batch_size: (j+1)*batch_size]

                # print(j*batch_size, (j+1)*batch_size)
                # print(batch_Data)
                sess.run(train_step, feed_dict = {xs: batch_Data, ys: batch_label, keep_prob: 0.5})
                print(compute_accuracy(test_Data,test_label,sess,prediction))
                if j % 50 == 0:
                    print(compute_accuracy(test_Data,test_label,sess,prediction))
        save_path = saver.save(sess, "./mymodel/model1.ckpt")


xs = tf.placeholder(tf.float32, [None, 10000])/255.
ys = tf.placeholder(tf.float32, [None, 3])
keep_prob = tf.placeholder(tf.float32)
x_image = tf.reshape(xs, [-1,100, 100, 1])

if __name__ == '__main__':
    name = 'Haotian'
    name1 = 'Gaojing'
    path = './datasets/'+ name
    path1 = './datasets/'+ name1 
    other = './datasets/otherface'
    images = []
    labels = []
    read_image(path, images, labels, 7000, name)
    read_image(path1, images, labels, 7000, name1)
    read_image(other, images, labels, 7000, 'None')
    images = np.array(images)
    labels = np.array(labels)
    p = np.random.permutation(len(images))
    images = images[p]
    labels = labels[p]
    print(labels)
    train(images, labels, 0.1, 0.0001, 2, 50)
