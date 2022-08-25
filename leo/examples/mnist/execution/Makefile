TARGET = test_mnist.elf

CFLAGS = -I../../../include -L../../../lib -fPIC -Wl,-rpath=../../../lib

CC = gcc
CFLAGS += -lgxdnn -lm -lpthread -lstdc++ # 用cmodel时libgxdnn库版本号需要和NPU文件一致

$(TARGET): test_mnist.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	-rm $(TARGET)
