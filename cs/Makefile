OBJECTS = 	util/cache.o \
		util/colorprint.o \
		util/getopt_helper.o

FLAGS = -Wall -g -O3 -march=native -std=gnu11

all: cachespeed

cachespeed: cachespeed.c $(OBJECTS) cachespeed.h
	gcc $(OBJECTS) $(FLAGS) -o $@ $@.c -lm

%.o: %.c
	gcc $(FLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) cachespeed
