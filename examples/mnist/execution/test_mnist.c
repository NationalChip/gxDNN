#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gxdnn.h"

int main(int argc, const char *argv[])
{
	FILE *image_fp;
	const char *model_name = "mnist.npu";
	const char *image_name;
	GxDnnResult ret;
	GxDnnDevice device;
	GxDnnTask task;
	int priority = 5;
	GxDnnEventHandler event_handler = NULL;
	int input_num, output_num;
	int input_size, output_size;
	GxDnnIOInfo *input, *output;
	int i;
	float image_buf[28*28];
	float *result;
	int result_num;
	int max_index;
	float max_value;

	if (argc < 2) {
		printf("Error: no input image.\n");
		return -1;
	}

	// 获取图像数据
	image_name = argv[1];
	image_fp = fopen(image_name, "rb");
	if (image_fp == NULL) {
		printf("Error: open image \"%s\" fail.\n", image_name);
		return -1;
	}
	fread(image_buf, sizeof(image_buf), 1, image_fp);
	fclose(image_fp);

	// 打开NPU设备
	GxDnnOpenDevice("/dev/gxnpu", &device);

	// 传入模型文件，得到模型task
	ret = GxDnnCreateTaskFromFile(device, model_name, &task);
	if (ret != GXDNN_RESULT_SUCCESS) {
		printf("Error: load model fail\n");
		return -1;
	}

	// 获取task的输入输出信息
	GxDnnGetTaskIONum(task, &input_num, &output_num);
	input_size = input_num * sizeof(GxDnnIOInfo);
	output_size = output_num * sizeof(GxDnnIOInfo);
	input = malloc(input_size);
	output = malloc(output_size);
	if (input == NULL || output == NULL) {
		printf("Error: malloc fail.\n");
		return -1;
	}
	GxDnnGetTaskIOInfo(task, input, input_size, output, output_size);

	// 拷贝输入数据到模型内存中
	memcpy(input[0].dataBuffer, (void*)image_buf, input[0].bufferSize);

	// 用同步方式运行模型
	GxDnnRunTask(task, priority, event_handler, NULL);
	result = output[0].dataBuffer;
	result_num = output[0].bufferSize/sizeof(float);
	max_value = -999999.99;
	for (i=0; i<result_num; i++) {
		if (result[i] > max_value) {
			max_value = result[i];
			max_index = i;
		}
	}
	printf("\nResult digit: %d\n", max_index);

	// 释放模型task
	GxDnnReleaseTask(task);

	// 关闭NPU设备
	GxDnnCloseDevice(device);

	free(input);
	free(output);

	return 0;
}

