CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = minisql
SRC = minisql.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
