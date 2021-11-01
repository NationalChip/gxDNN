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
	with tf.gfile.FastGFile('./mnist_with_ckpt.pb', mode='wb') as f:
	    f.write(constant_graph.SerializeToString())


For the modified document, see [here](./compilation/mnist.py)。
After running `python mnist.py`, `mnist_with_ckpt.pb` will be generated under the current path.

### Edit the NPU Config File ###

Edit the config file [config.yaml](./compilation/config.yaml):

	CORENAME: GRUS # chip core name
	PB_FILE: mnist_with_ckpt.pb # input pb file name
	OUTPUT_FILE: model.h # output NPU file name
	NPU_UNIT: NPU32 # NPU MAC type
	COMPRESS: true # compress FC weight
	CONV2D_COMPRESS: true # compress Conv2D weight
	OUTPUT_TYPE: c_code # output NPU file format
	INPUT_OPS:
	        input_x: [1, 784] # input node name and shape
	OUTPUT_OPS: [result] # output node name

### Compile ###

Compile using `gxnpuc`
```
gxnpuc config.yaml
```

Generate `model.h`.


## Run Model ##

After the NPU file is generated, we need to call the API provided by NationalChip to deploy the model to the gx8002 development board.

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

