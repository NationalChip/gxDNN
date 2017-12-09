#coding: utf-8
# NHWC --> NCHW
# 只需要对第一个输入数据和全连接的权重做transpose就行了

# example:
# W0:[5,5,2,32] b0:[32] W1:[768,64] b1:[64]

import tensorflow as tf
import numpy as np

########################## weights ##############################
batch_size = 1
input_shape = [1,8,40,2]
feats = tf.placeholder(tf.float32, shape=input_shape, name="feed")
w0_data = [float(i%20)/5/5 for i in range(5*5*2*32)]
b0_data = [float(i%4)/2/3 for i in range(32)]
w1_data = [float((i+3)%8)/2/5 for i in range(768*64)]
b1_data = [float(i%3)/5 for i in range(64)]
W0 = tf.constant(w0_data, shape=[5,5,2,32])
b0 = tf.constant(b0_data, shape=[32])
W1 = tf.constant(w1_data, shape=[768,64])
b1 = tf.constant(b1_data, shape=[64])

########################## model ##############################
'''
# 原始代码 (NHWC)
x = feats # [batch_size,8,40,2]
h_conv = tf.nn.relu(tf.nn.conv2d(x, W0, strides=[1, 1, 1, 1], padding='VALID') + b0) # [batch_size,4,36,32]
h_pool = tf.nn.max_pool(h_conv, ksize=[1, 2, 3, 1], strides=[1, 2, 3, 1], padding='VALID') # [batch_size,2,12,32]
h_pool_flat = tf.reshape(h_pool, [batch_size, -1]) # [batch_size,768]
y = tf.nn.relu(tf.matmul(h_pool_flat, W1) + b1) # [batch_size,64]
'''

# 修改后代码 (NCHW)
x = feats # [batch_size,8,40,2]
x = tf.transpose(x, [0, 3, 1, 2]) # 输入数据需要做transpose
b0 = tf.reshape(b0, [32, 1, 1]) # [32,1,1]
h_conv = tf.nn.relu(tf.nn.conv2d(x, W0, strides=[1, 1, 1, 1], padding='VALID', data_format='NCHW') + b0) # [batch_size,32,4,36]
h_pool = tf.nn.max_pool(h_conv, ksize=[1,1,2, 3], strides=[1, 1, 2, 3], padding='VALID', data_format='NCHW') # [batch_size,32,2,12]
# 如果还有其他conv2d或max_pool操作，不需要再做transpose
# ...
W1 = tf.reshape(W1, [24, 32, 64])
W1 = tf.transpose(W1, [1, 0, 2]) # 全连接权重需要做transpose
W1 = tf.reshape(W1, [24*32, 64])
h_pool_flat = tf.reshape(h_pool, [batch_size,-1]) # [batch_size,768]
y = tf.nn.relu(tf.matmul(h_pool_flat, W1) + b1) # [batch_size,64]


########################## test ##############################
with tf.Session() as sess:
    feed_data = [float((i+4)%9)/10 for i in range(8*40*2)]
    feed_data = np.reshape(feed_data, input_shape)
    res = sess.run(y, feed_dict={feats: feed_data})
    print res

