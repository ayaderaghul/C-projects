CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = bt
SRC = bt.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
