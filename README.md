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

#### 配置文件说明 ####

| 配置项              | 选项                 | 说明                                                    |
| ------------------- | -------------------  | ------------------------------------------------------- |
| CORENAME            | LEO / LEO_MPW        | 芯片型号                                                |
| PB_FILE             |                      | 包含 CKPT 的 PB 文件                                    |
| OUTPUT_FILE         |                      | 编译后生成的文件名                                      |
| NPU_UNIT            | NPU32 / NPU64        | NPU 型号对应的 MAC 数（SNPU 选 NPU32，主 NPU 选 NPU64） |
| COMPRESS            | true / false         | 是否启动压缩模式                                        |
| COMPRESS_QUANT_BITS | 4/5/6/7/8            | 量化压缩的 bit 数                                       |
| COMPRESS_TYPE       | LINEAR / GAUSSIAN    | 线性压缩还是高斯压缩                                    |
| OUTPUT_TYPE         | raw / c_code         | Linux 环境下运行的模型请选择 raw                        |
| DEBUG_INFO_ENABLE   | true / false         | 编译的输出文件中是否带调试信息（默认 false）            |
| INPUT_OPS           | op_name: [shape] ... | 设置输入的 OP 名和 shape                                |
| OUTPUT_OPS          | [out_op_names, ...]  | 设置输出的 OP 名列表                                    |
| INPUT_DATA          | op_name: [data] ...  | 有些 Placeholder 的数据在部署时是确定的，需要写明       |

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

