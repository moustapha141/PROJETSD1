CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
LDFLAGS = -lm

all: projet benchmark

projet: PROJETSD1.c
	$(CC) $(CFLAGS) -o projet PROJETSD1.c $(LDFLAGS)

benchmark: benchmark.c
	$(CC) $(CFLAGS) -o benchmark benchmark.c $(LDFLAGS)

run: projet
	./projet

run-benchmark: benchmark
	./benchmark

clean:
	rm -f projet benchmark *.o *.dat

.PHONY: all run run-benchmark clean
