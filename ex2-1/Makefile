CC = gcc
SRC = ex2-1
TARGET = $(SRC).o
OBJS = ../shared/*.c $(SRC).c

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(TARGET)
