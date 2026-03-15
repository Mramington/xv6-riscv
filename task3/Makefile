CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGET = task3
SRC = task3.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: clean