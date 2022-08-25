# MNIST

Now take MNIST model as an example to illustrate the use of gxDNN.

## Generate NPU File ##

### Generate Frozen PB ###

In order to easily obtain the input node and output node, we need to name the input node and output node:

change
	x = tf.placeholder(tf.float32, [None, 784])

to

	x = tf.placeholder(tf.float32, [None, 784], name="input_x")

change

	y = tf.add(tf.matmul(x, W), b)

to

	y = tf.add(tf.matmul(x, W), b, name="result")

To generate a PB file, add the following at the end of the file:

	from tensorflow.python.framework import graph_util
	constant_graph = graph_util.convert_variables_to_constants(sess, sess.graph_def, ["result"])
	with tf.gfile.FastGFile('./generated_pb/mnist_with_ckpt.pb', mode='wb') as f:
	    f.write(constant_graph.SerializeToString())


For the modified document, see [here](./compilation/mnist.py)。
After running `python mnist.py`, `mnist_with_ckpt.pb` will be generated under 'generated_pb' file.

### Edit the NPU Config File ###

Edit the config file [config.yaml](./compilation/config.yaml):

    CORENAME: LEO                               # 芯片型号 LEO/LEO_MPW
    PB_FILE: ./generated_pb/mnist_with_ckpt.pb  # 输入的pb文件
    OUTPUT_FILE: mnist.npu                      # 输出的NPU文件名
    NPU_UNIT: NPU64                             # NPU设备类型
    COMPRESS: false                             # 压缩模型
    COMPRESS_QUANT_BITS: 8                      # 量化成8bits
    COMPRESS_TYPE: LINEAR                       # 线性压缩
    OUTPUT_TYPE: raw                            # NPU文件的类型
    INPUT_OPS:
            input_x: [1, 784]                   # 输入结点名称和数据维度，每运行一次输入数据为1x784，即一幅图
    OUTPUT_OPS: [result]                        # 输出结点名称 

### Compile ###

Compile using `gxnpuc`
```
gxnpuc config.yaml
```

Generate `mnist.npu`.


## Run Model ##

After the NPU file is generated, we need to call the API provided by NationalChip to deploy the model to the gx8010 development board.

### Call API Process ###

1. Open NPU device.
2. Load the model and input data, run the model and get the output data.
3. Close NPU device.

### MNIST Example ###

Please refer to the code [here](./execution/test_mnist.c)。The program requires the user to input a binary file with 28x28 pixel values and output the recognized number.

Several binary test files are stored in the directory [images](./execution/images). The content is the normalized pixel value of 28x28.

Execute ` make ` generate executable `test_mnist.elf`.

Run `test_mnist.elf`, the results are as follows:

    ./test_mnist.elf images/image0
    Digit: 7
    ./test_mnist.elf images/image1
    Digit: 2
    ./test_mnist.elf images/image2
    Digit: 1
    ./test_mnist.elf images/image3
    Digit: 0
    ./test_mnist.elf images/image4
    Digit: 4
    ./test_mnist.elf images/image5
    Digit: 1

