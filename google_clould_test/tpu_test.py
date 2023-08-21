#######p############################################
#filename      : tpu_test.py
#author        : litao
#e-mail        : 362085095@qq.com
#create time   : 2022-10-08 10:36:33
#last modified : 2022-10-08 10:36:33
####################################################
#!/usr/bin/env python

import tensorflow as tf
print("Tensorflow version " + tf.__version__)

@tf.function
def add_fn(x,y):
  z = x + y
  return z

cluster_resolver = tf.distribute.cluster_resolver.TPUClusterResolver()
tf.config.experimental_connect_to_cluster(cluster_resolver)
tf.tpu.experimental.initialize_tpu_system(cluster_resolver)
strategy = tf.distribute.TPUStrategy(cluster_resolver)

x = tf.constant(1.)
y = tf.constant(1.)
z = strategy.run(add_fn, args=(x,y))
print(z)
