CC = gcc

COMMON_CFLAGS = -Wall -pthread -I./include -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
LDFLAGS = -lm -lncursesw

# --- FILE PATHS ---
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

TARGET = $(BUILD_DIR)/tcas-ii-sim

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Default build profile
all: release

# -- BUILD PROFILES --

# DEBUG: optimization off, symbols on, ThreadSanitizer on
debug: CFLAGS = $(COMMON_CFLAGS) -g -O0 -fsanitize=thread
debug: directories $(TARGET)

# RELEASE: optimization on, asserts off, ThreadSanitizer off
release: CFLAGS = $(COMMON_CFLAGS) -O2 -DNDEBUG
release: directories $(TARGET)

# -- RULES --
directories:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean directories debug release
