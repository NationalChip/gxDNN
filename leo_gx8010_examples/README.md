# npu_simulate #

在 PC 上模拟 NPU 的执行。可以用来评估模型的计算结果。

## PC 上需要的库和头文件

位于 ./release/nre 目录下

## ARM 上需要的库和头文件

位于 ./release/arm_nre 目录下

## 编译运行 MNIST ##

1. cd mnist
2. 修改Makefile，根据在 PC 上仿真还是 ARM 上运行，选择 CHIP 为 nre 或 arm_nre
3. make
4. 如果是 ARM 上运行，把 mnist.npu, images/ 和 ./release/arm_nre/libgxdnn.so 拷贝到 ARM 端，并设置动态库环境变量： export LD_LIBRARY_PATH=`pwd`
5. ./test_mnist.elf images/image0

