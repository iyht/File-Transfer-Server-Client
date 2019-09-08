import tensorflow as tf
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
import cv2
import os
from sklearn.model_selection import train_test_split
import dlib






def compute_accuracy(v_xs, v_ys):
    prediction
    y_pre = sess.run(prediction, feed_dict = {xs: v_xs, keep_prob: 1})
    correct_prediction = tf.equal(tf.argmax(y_pre, 1), tf.argmax(v_ys,1))
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))
    result = sess.run(accuracy, feed_dict={xs: v_xs, ys: v_ys, keep_prob: 1})
    return result

def weight_variable(shape):
    initial = tf.truncated_normal(shape, stddev = 0.1)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.1, shape = shape)
    return tf.Variable(initial)

def conv2d(x, W):
    # stride [1, x_movement, y_movement, 1]
    # Must have strides[0] = strides[3] = 1
    return tf.nn.conv2d(x, W, strides=[1,1,1,1], padding = 'SAME')

def max_pool_2x2(x):
    # stride [1, x_movement, y_movement, 1]
    return tf.nn.max_pool(x, ksize = [1,2,2,1], strides = [1,2,2,1], padding='SAME')

def is_my_face(image):  
    res = sess.run(prediction, feed_dict={xs: [image], keep_prob: 1})  
    print(res)
    if res[0][0] >= res[0][1]:  
        return True  
    else:  
        return False  
xs = tf.placeholder(tf.float32, [None, 10000])/255.
ys = tf.placeholder(tf.float32, [None, 2])
keep_prob = tf.placeholder(tf.float32)
x_image = tf.reshape(xs, [-1,100, 100, 1])

W_conv1 = weight_variable([2, 2, 1,32])
b_conv1 = bias_variable([32])
h_conv1 = tf.nn.relu(conv2d(x_image, W_conv1) + b_conv1) # output size = 28x28x32, 100x100x32
h_pool1 = max_pool_2x2(h_conv1)                          # output size = 14x14x32, 50x50x32


W_conv2 = weight_variable([2,2,32,64])  #patch 5x5, in size(layers) 32, out size 64
b_conv2 = bias_variable([64])
h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2) + b_conv2) # output size 14x14x64, 50x50x64
h_pool2 = max_pool_2x2(h_conv2)                          # output size 7x7x64, 25x25x64



W_fc1 = weight_variable([25*25*64, 1024])
b_fc1 = bias_variable([1024])
h_pool2_flat = tf.reshape(h_pool2, [-1, 25*25*64])
h_fc1 = tf.nn.relu(tf.matmul(h_pool2_flat, W_fc1)+ b_fc1)
h_fc1_drop = tf.nn.dropout(h_fc1, rate = 1-keep_prob)


W_fc2 = weight_variable([1024, 2])
b_fc2 = bias_variable([2])
logits = tf.add(tf.matmul(h_fc1_drop, W_fc2),b_fc2)
prediction = tf.nn.softmax(logits)

saver = tf.train.Saver()
with tf.Session() as sess:
    saver.restore(sess, "./mymodel/model1.ckpt")
    detector = dlib.get_frontal_face_detector()

    cam = cv2.VideoCapture(0)  
    while True:  
        _, img = cam.read()  
        gray_image = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        dets = detector(gray_image, 1)
        if not len(dets):
            #print('Can`t get face.')
            cv2.imshow('img', img)
            key = cv2.waitKey(30) & 0xff  
            if key == 27:
                sys.exit(0)
                
        for i, d in enumerate(dets):
            x1 = d.top() if d.top() > 0 else 0
            y1 = d.bottom() if d.bottom() > 0 else 0
            x2 = d.left() if d.left() > 0 else 0
            y2 = d.right() if d.right() > 0 else 0
            face = gray_image[x1:y1,x2:y2]
            face = cv2.resize(face, (100,100))
            face = np.ravel(face)
            print('Is this my face? %s' % is_my_face(face))

            cv2.rectangle(img, (x2,x1),(y2,y1), (255,0,0),3)
            cv2.imshow('image',img)
            key = cv2.waitKey(30) & 0xff
            if key == 27:
                sys.exit(0)