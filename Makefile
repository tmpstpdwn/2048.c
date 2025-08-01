CC = gcc
LDFLAGS = -lraylib -lm -ldl -lrt -lpthread -lX11

SRC = main.c 2048.c
OBJ = $(SRC:.c=.o)
TARGET = 2048

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
