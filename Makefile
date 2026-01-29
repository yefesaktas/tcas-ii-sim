CC = gcc
CFLAGS = -Wall -pthread -g -fsanitize=thread # ThreadSanitizer is active, deactivate before production
LDFLAGS = -lm
TARGET = tcas-ii-sim
SRCS = main.c storage.c transponder_data.c tracker.c tcas_logic.c display.c audio.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
