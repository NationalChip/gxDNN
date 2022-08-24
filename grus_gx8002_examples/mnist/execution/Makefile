TARGET = test_mnist.elf

CFLAGS = -I../../include -L../../lib

CC = gcc
CFLAGS += -ldriver -lcmodel -lm -lpthread -lstdc++

$(TARGET): test_mnist.c
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	-rm $(TARGET)
