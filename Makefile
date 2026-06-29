CC = gcc
CFLAGS = -Wall -pthread -g -fsanitize=thread -I./include -D_POSIX_C_SOURCE=200809L # ThreadSanitizer is active, deactivate before production

LDFLAGS = -lm -lncursesw

# --- FILE PATHS ---
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

TARGET = $(BUILD_DIR)/tcas-ii-sim

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: directories $(TARGET)

directories:
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean directories
