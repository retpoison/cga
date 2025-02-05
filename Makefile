CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -Wno-unused-parameter -O2

all: main

ga.o: ga.c
	$(CC) -o $@ -c $(CFLAGS) $^

main:	main.c ga.o
	$(CC) $(CFLAGS) -o $@ -s $^

clean:
	rm -vf main *.o
