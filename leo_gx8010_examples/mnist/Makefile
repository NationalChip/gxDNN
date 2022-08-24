TARGET = test_mnist.elf

CHIP = nre
#CHIP = arm_nre

CFLAGS = -I../release/$(CHIP) -L../release/$(CHIP) -fPIC -Wl,-rpath=../release/$(CHIP)

ifeq ($(CHIP), arm_nre)
	CC = arm-linux-gnueabihf-gcc
	CFLAGS += -lgxdnn -lm -lpthread 
else
	CC = gcc
	CFLAGS += -lgxdnn_1_0_15 -lm -lpthread -lstdc++ # 用cmodel时libgxdnn库版本号需要和NPU文件一致
endif

$(TARGET): test_mnist.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	-rm $(TARGET)
