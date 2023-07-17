
CC = gcc
CFLAGS = -std=c99 -fPIC -Wall -Wextra -pedantic \
		 -Wfloat-equal -Wshadow -Wno-unused-parameter -Wl,--export-dynamic \
		 -Wswitch-enum -Wcast-qual -Wnull-dereference -Wunused-result # -Waggregate-return
INCLUDES = -Isrc/
LIBS = -lm -llua

SRC = src/main.c src/luatml.c src/luatml_build.c src/luatml_fs.c
OBJ = $(SRC:.c=.o)
TARGET = luatml

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

