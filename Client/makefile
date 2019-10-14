
CC = gcc
CFLAGS = -Wall
DEPS = networking.h
SRC_FILES = networking.c fileClient.c
TARGET = fileClient

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES) $(DEPS)

clean:
	rm -f $(TARGET)

