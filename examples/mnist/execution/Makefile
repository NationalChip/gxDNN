CC = arm-linux-gnueabihf-gcc
TARGET = test_mnist.elf

$(TARGET): test_mnist.c
	$(CC) -Wall test_mnist.c -L. -lgxdnn -lm -o $@

clean:
	-rm $(TARGET)
