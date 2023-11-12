CC = gcc
CFLAGS = -Wall -Wextra
SRCS = banker.c activityQueue.c list.c
TARGET = banker

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	rm -f $(TARGET)