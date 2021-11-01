#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "gx_snpu.h"
#include "model.h"

static struct input in;
static struct output out;

static int is_finished = 0;

#define MCU_TO_DEV(x) (x)

static unsigned short float_32_to_16(float in_data, int exponent_width)
{
	int e_bit_width = exponent_width;
	int d_bit_width = 15 - e_bit_width;
	unsigned int s, e, d;
	int e_out, d_out;
	unsigned short out_data;
	unsigned char round_bit;
	unsigned int u32_in_data = *(unsigned int *)&in_data;

	s = (u32_in_data & 0x80000000) >> 31;
	e = (u32_in_data & 0x7f800000) >> 23;
	d = (u32_in_data & 0x007fffff);
	e_out = e - (1 << 7) + (1 << (e_bit_width - 1));
	d_out = (d >> (23 - d_bit_width));
	round_bit = ((d >> (23 - d_bit_width - 1)) & 1);
	if (e_out < 0) // overflow
		out_data = (s << 15) | 0;
	else if (e_out >= (1 << e_bit_width) - 1) // overflow
		out_data = (s << 15) | 0x7fff;
	else
		out_data = (s << 15) | (((e_out << d_bit_width) | d_out) + round_bit);

	return out_data;
}

static float float_16_to_32(unsigned short in_data, int exponent_width)
{
	int e_bit_width = exponent_width;
	unsigned int s, e, d;
	int e_out, d_out;
	unsigned int out;

	s = (in_data & 0x8000) >> 15;
	e = (in_data >> (15 - e_bit_width)) & ((1 << e_bit_width) - 1);
	d = (in_data & ((1 << (15 - e_bit_width)) - 1));
	if (e == 0 && d == 0) { // float 0.0: 0
		e_out = 0;
		d_out = 0;
	} else if (e == (1 << e_bit_width) - 1) { // overflow
		e_out = 0xff;
		d_out = 0;
	} else {
		e_out = e + (1 << 7) - (1 << (e_bit_width - 1));
		d_out = d << (e_bit_width + 8);
	}
	out = (s << 31) | (e_out << 23) | d_out;
	return *(float*)&out;
}

static int snpu_float32_to_16(GX_SNPU_FLOAT *out_data, float *in_data, int num)
{
	int i;

	if (in_data == NULL || out_data == NULL) {
		printf("[%s] error: para null\n", __FUNCTION__);
		return -1;
	}

	for (i=0; i<num; i++) {
		out_data[i] = float_32_to_16(in_data[i], 5);
	}

	return 0;
}

static int snpu_float16_to_32(float *out_data, GX_SNPU_FLOAT *in_data, int num)
{
	int i;

	if (in_data == NULL || out_data == NULL) {
		printf("[%s] error: para null\n", __FUNCTION__);
		return -1;
	}

	for (i=0; i<num; i++) {
		out_data[i] = float_16_to_32(in_data[i], 5);
	}

	return 0;
}

static int callback(int module_id, GX_SNPU_STATE state, void *private_data)
{
	int index = *(int*)private_data;
	int i;
	float *out_data;

	// print result
	int max_index = -1;
	float max_value = -999999.99;
	out_data = (float*)&out.result;
	for (i = 0; i < sizeof(out.result)/sizeof(float); i++) {
		if (out_data[i] > max_value) {
			max_value = out_data[i];
			max_index = i;
		}
	}
	printf("Result digit: %d\n", max_index);

	is_finished = 1;
	return 0;
}

int main(int argc, const char *argv[])
{
	int index = 0;
	int ret;

	// 获取图像数据
	if (argc < 2) {
		printf("Error: please input image file.\n");
		return -1;
	}
	FILE *image_fp;
	const char *image_name;
	float image_buf[28*28];
	image_name = argv[1];
	image_fp = fopen(image_name, "rb");
	if (image_fp == NULL) {
		printf("Error: open image \"%s\" fail.\n", image_name);
		return -1;
	}
	fread(image_buf, sizeof(image_buf), 1, image_fp);
	fclose(image_fp);

	// init snpu
	gx_snpu_init();
	// run snpu
	GX_SNPU_TASK task;
	task.module_id = 3;
	task.ops = (void*)MCU_TO_DEV(ops_content);
	task.data = (void*)MCU_TO_DEV(data_content);
	task.cmd = (void*)MCU_TO_DEV(cmd_content);
	task.tmp_mem = (void*)MCU_TO_DEV(tmp_content);
	task.weight = (void*)MCU_TO_DEV(weight_content);
	task.input = (void*)MCU_TO_DEV(&in);
	task.output = (void*)MCU_TO_DEV(&out);
	is_finished = 0;
	snpu_float32_to_16((GX_SNPU_FLOAT*)&in.input_x, image_buf, sizeof(in.input_x)/sizeof(GX_SNPU_FLOAT));
	ret = gx_snpu_run_task(&task, callback, (void*)&index);
	if (ret) {
		printf("gx_snpu_run_task error %d\n", ret);
		return -1;
	}
	while (!is_finished); // wait snpu done
	// exit snpu
	gx_snpu_exit();
	return 0;
}

