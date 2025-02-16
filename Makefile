CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wno-unused-parameter -O2

EX = example1 example2

all: $(EX)

ga.o: ga.c
	$(CC) -o $@ -c $(CFLAGS) $^

$(EX): ga.o
	$(CC) $(CFLAGS) -o $@ -s $^ $@.c

clean:
	rm -vf $(EX) *.o
