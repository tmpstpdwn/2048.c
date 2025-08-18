# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
ASSETS_DIR = assets

# Sources
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Linux build settings
LINUX_RAYLIB_DIR = vendor/raylib-linux
CC = gcc
CFLAGS = -I$(INC_DIR) -I$(LINUX_RAYLIB_DIR)/include
LDFLAGS = -L$(LINUX_RAYLIB_DIR)/lib -lraylib -lm -ldl -lrt -lpthread -lX11
TARGET = 2048

# Windows cross compile settings
WIN_CC = x86_64-w64-mingw32-gcc
WIN_RAYLIB_DIR = vendor/raylib-crosswin
WIN_CFLAGS = -I$(INC_DIR) -I$(WIN_RAYLIB_DIR)/include
WIN_LDFLAGS = -L$(WIN_RAYLIB_DIR)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
WIN_TARGET = 2048.exe

.PHONY: all clean crosswin

# Default Linux build
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(WIN_TARGET)

# ----- Windows Build -----

crosswin: $(WIN_TARGET)

$(WIN_TARGET): $(SRC)
	$(WIN_CC) $(WIN_CFLAGS) $(SRC) -o $(WIN_TARGET) $(WIN_LDFLAGS)
