CFLAGS=-O2 -marm -std=c99
LDFLAGS=-marm
CC:=/home/jinmo/cross-compiler-armv5l/bin/armv5l-gcc
# CFLAGS=-fsanitize=address -g
# LDFLAGS=-fsanitize=address -g

# CFLAGS=-g
# LDFLAGS=-g

all: main Makefile

main: main.o

main.o: main.c

clean:
	rm main.o main -f
