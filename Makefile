
CC = gcc
CFLAGS = -std=c99 -fPIC -Wall -Wextra -pedantic \
		 -Wfloat-equal -Wshadow -Wno-unused-parameter \
		 -Wswitch-enum -Wcast-qual -Wnull-dereference -Wunused-result # -Waggregate-return
INCLUDES = -Isrc/ -I/data/data/com.termux/files/usr/include/lua5.3
LIBS = -lm -llua5.3 -lmicrohttpd

SRC = src/main.c src/luatml.c src/luatml_build.c src/luatml_serve.c src/luatml_fs.c src/luatml_lib.c
OBJ = $(SRC:.c=.o)
TARGET = luatml

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

