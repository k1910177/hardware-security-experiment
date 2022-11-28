CC = gcc
SRC = idea
TARGET = $(SRC).o
OBJS = ../utils/*.c $(SRC).c

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

clean:
	rm -rf $(TARGET)
