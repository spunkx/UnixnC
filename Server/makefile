
CC = gcc
CFLAGS = -Wall -pthread -lpthread
DEPS = logger.h networking.h authentication.h
SRC_FILES = logger.c networking.c fileServer.c authentication.c
TARGET = fileServer

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES) $(DEPS)

clean:
	rm -f $(TARGET)

