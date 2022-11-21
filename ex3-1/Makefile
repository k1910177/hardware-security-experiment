CC = gcc
SRC = ex3-1
TARGET = $(SRC).o
OBJS = ../utils/transformation.c ../utils/debug.c $(SRC).c

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(TARGET)
