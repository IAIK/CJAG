OBJECTS = 	detection/paging.o \
		detection/cache.o \
		detection/cpu.o \
		util/error.o \
		util/timing.o \
		util/colorprint.o \
		util/getopt_helper.o \
		util/watchdog.o \
		cache/evict.o \
		cache/set.o \
		cache/slice.o \
		jag/send.o \
		jag/common.o \
		jag/receive.o
		
FLAGS = -Wall -g -O3 -march=native -std=gnu11 -pthread

all: cjag

cjag: cjag.c $(OBJECTS) cjag.h
	gcc $(OBJECTS) $(FLAGS) -o $@ $@.c -lm

%.o: %.c
	gcc $(FLAGS) -c $^ -o $@
	
.PHONY: clean
clean:
	rm -f $(OBJECTS) cjag
