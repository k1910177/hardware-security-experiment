CC = gcc
SRC = ex4
TARGET = $(SRC).o
OBJS = ../shared/*.c $(SRC).c

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(TARGET)
