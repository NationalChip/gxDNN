# gxDNN #

Run Neural networks on NationalChip NPU processor.

## 安装 ##

### 安装gxDNN工具链 ###

> pip install npu_compiler

### 更新gxDNN工具链 ###

> pip install --upgrade npu_compiler

### 查看工具链版本 ###

安装或更新完成后，可以使用如下命令查看当前工具链的版本号。

> gxnpuc --version

## 工具链使用 ##

### gxnpuc ###

用于把模型文件编译成能在 NPU 上运行的 npu 文件。

	usage: gxnpuc [-h] [-V] [-L] [config_filename]
	
	  -h, --help       show this help message and exit
	  -V, --version    show program's version number and exit
	  -L, --list       list supported ops

### gxnpudebug ###

如果编译时配置文件中的 DEBUG_INFO_ENABLE 选项设置为 true，编译出的 npu 文件带上了调试信息，此时可以使用调试工具 gxnpudebug 工具来处理该文件。

	usage: gxnpudebug [-h] [-S] [-P] file [file ...]
	
	optional arguments:
	  -h, --help        show this help message and exit
	  -P, --print_info  print debug info
	  -S, --strip       strip debug info

### gxnpu_rebuild_ckpt ###

对权重数据做量化或做float16，并重新生成 ckpt 文件，用于评估模型压缩后对结果的影响。

## 举例 ##

[MNIST](examples/mnist "MNIST")

## 优化 ##

为了让模型更高效地运行在 NPU 处理器上，需要对模型做一些优化。

- 做卷积和降采样的数据格式需要用 NCHW 格式，优化过程可以参考[这里](examples/optimization/nhwc_2_nchw.py)。
- Placeholder 的维度信息需要确定。
- 各 OP 的 shape 需要确定，即和 shape 有关的 OP value 需要确定。

