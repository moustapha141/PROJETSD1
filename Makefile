CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
LDFLAGS = -lm

SRC  = src/PROJETSD1.c
EXEC = projetsd1

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(EXEC) $(LDFLAGS)

# Cible dediee aux benchmarks : compile en -O2 (indispensable, le tri par
# insertion en O(n^2) sur n=100000 est sinon beaucoup trop lent) puis lance
# directement la collecte des mesures dans resultats_benchmark.csv.
benchmark: $(SRC)
	$(CC) $(CFLAGS) -O2 $(SRC) -o $(EXEC) $(LDFLAGS)
	./$(EXEC) --benchmark

clean:
	rm -f $(EXEC) *.dat resultats_benchmark.csv

.PHONY: all clean benchmark
