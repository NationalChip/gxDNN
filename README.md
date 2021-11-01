# gxDNN #

Run Neural networks on NationalChip NPU processor.

## Install ##

### Install gxDNN Compiler ###

> pip install npu_compiler

### Upgrade gxDNN Compiler ###

> pip install --upgrade npu_compiler

### View Compiler Version ###

> gxnpuc --version

## Compiler Usage ##

### gxnpuc ###

Compile TF model file into NPU file that can run on NPU.

	usage: gxnpuc [-h] [-V] [-L] [-v] [-m] [-c CMD [CMD ...]] [-w]
	              [config_filename]
	
	NPU Compiler
	
	positional arguments:
	  config_filename       config file
	
	optional arguments:
	  -h, --help            show this help message and exit
	  -V, --version         show program's version number and exit
	  -L, --list            list supported ops
	  -v, --verbose         verbosely list the processed ops
	  -m, --meminfo         verbosely list memory info of ops
	  -c CMD [CMD ...], --cmd CMD [CMD ...]
	                        use command line configuration
	  -w, --weights         print compressed weights(GRUS only)

#### Config File Description ####

| ITEM                | Configuration        | Description                                             |
| ------------------- | -------------------  | ------------------------------------------------------- |
| CORENAME            | GRUS                 | Chip name.                                              |
| PB_FILE             |                      | TensorFlow Frozen PB.                                   |
| OUTPUT_FILE         |                      | Output file name.                                       |
| NPU_UNIT            | NPU32                | NPU MAC num. (Must be NPU32 on GRUS)                    |
| COMPRESS            | true / false         | FC weights compress or not.                             |
| CONV2D_COMPRESS     | true / false         | Conv2D weights compress or not.                         |
| OUTPUT_TYPE         | c_code               | Output type. (Must be c_code on GRUS)                   |
| INPUT_OPS           | op_name: [shape] ... | Input OP name and shape.                                |
| OUTPUT_OPS          | [out_op_names, ...]  | Output OP name.                                         |
| FP16_OUT_OPS        | [out_op_names, ...]  | Output OP name with fp16 format. (Others use fp32)      |
| FUSE_BN             | true / false         | Fuse batch nomailization or not.                        |

## Examples ##

[MNIST](examples/mnist "MNIST")

## FAQ ##

	Q: What is the Python version supported by the NPU Compiler?
	A: Python 2.7 and Python 3.6 are supported.
	
	Q: What is the TensorFlow version supported by the NPU Compiler?
	A: All versions above TensorFlow 1.10 are supported.
	
	Q: How do I view the list of OP supported by NPU Compiler?
	A: Use the command `gxnpuc --list`. See GRUS_OPS content for GX8002 chip.
	
	Q: How do I view the version of the NPU Compiler?
	A: Use the command `gxnpuc --version`.
	
	Q: "Some OP types are not supported when compiling the model!" What if this error is encountered while compiling the model?
	A: Consult the engineers of NationalChip to see if they can add the printed unsupported OP.
	
	Q: Is dynamic RNN model supported?
	A: Not supported. The model structure needs to be modified and implemented with a for loop.

