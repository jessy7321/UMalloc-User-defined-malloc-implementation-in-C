CC = gcc
CCFLAGS = -Wall

all: memgrind

memgrind: memgrind.c umalloc.c umalloc.h
	$(CC) $(CCFLAGS) -g -o memgrind memgrind.c

clean:
	rm memgrind
