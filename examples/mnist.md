# MNIST

下面以TensorFlow自带的[mnist](https://github.com/tensorflow/tensorflow/blob/v1.0.0/tensorflow/examples/tutorials/mnist/mnist_softmax.py "mnist")模型为例，说明gxDNN的使用。

## 生成NPU文件 ##

MNIST是一个入门级的计算机视觉数据集，它的输入是像素为28x28的手写数字图片，输出是图片对应的0-9数字的概率。
这个MNIST计算模型非常简单，可以用一个公式来表示： `y = x * W + b`
其中`x`为输入数据，`y`为输出数据，`W`和`b`为训练的参数。训练的过程就是不断通过计算出来的`y`和期望的`y_`去调整`W`和`b`的过程。
在NPU上，我们只需要用到训练好的`W`和`b`，而不需要训练的过程。

### 生成ckpt和pb文件 ###

为了方便的获取到输入结点和输出结点，我们给输入结点和输出结点取个名字：
把`mnist_softmax.py`第40行
> x = tf.placeholder(tf.float32, [None, 784])

修改为
> x = tf.placeholder(tf.float32, [None, 784], name="input_x")

把第43行
> y = tf.matmul(x, W) + b

修改为
> y = tf.add(tf.matmul(x, W), b, name="result")

为了生成pb和ckpt文件，在文件末尾添加：

    saver = tf.train.Saver() 
    saver.save(sess, "mnist.ckpt") 
    tf.train.write_graph(sess.graph_def, "./", "mnist.pb")

运行程序后，当前路径下会生成`mnist.ckpt`和`mnist.pb`文件。

### 把ckpt和pb文件合并成一个pb文件 ###

使用[freeze_graph.py](https://github.com/tensorflow/tensorflow/blob/v1.0.0/tensorflow/python/tools/freeze_graph.py "freeze_graph.py")脚本将`mnist.ckpt`和`mnist.pb`合并为一个pb文件。
注意：不同TensorFlow版本的`freeze_graph.py`脚本可能不同。

执行命令
> python freeze_graph.py --input_graph=mnist.pb --input_checkpoint=./mnist.ckpt --output_graph=mnist_with_ckpt.pb --output_node_names=result

生成`mnist_with_ckpt.pb`文件。
其中,`--input_graph`后跟输入pb名，`--input_checkpoint`后跟输入ckpt名，`--output_graph`后跟合成的pb文件名，`--output_node_names`后跟输出结点名称，如有多个，用逗号分隔。
执行完成后，在当前路径下生成`mnist_with_ckpt.pb`文件。

### 编辑NPU配置文件 ###

编辑配置文件`mnist_config.yaml`文件，含义见注释。

    PB_FILE: mnist_with_ckpt.pb # 输入的pb文件
    OUTPUT_FILE: mnist.npu # 输出的NPU文件名
    SECURE: false # 不开启内容保护
    NPU_UNIT: NPU32 # NPU设备类型
    COMPRESS: true # 压缩模型
    COMPRESS_QUANT_BITS: 8 # 量化成8bits
    OUTPUT_TYPE: raw # NPU文件的类型
    INPUT_OPS:
        input_x: [1, 784] # 输入结点名称和数据维度，每运行一次输入数据为1x784，即一幅图
    OUTPUT_OPS: [result] # 输出结点名称

### 编译 ###

使用gxDNN工具链编译
> gxnpuc --config=./mnist_config.yaml

生成NPU文件`mnist.npu`


## 执行NPU文件 ##


