CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LDFLAGS = -lm

SRC  = src/PROJETSD1.c
EXEC = projetsd1

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

clean:
	rm -f $(EXEC) *.dat

.PHONY: all clean
