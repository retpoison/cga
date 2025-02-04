#ifndef __SGA__
#define __SGA__

#include "bitset.h"

#define RAND01 ((double)rand() / (double)((unsigned)RAND_MAX + 1))

/*  population  */
#define POP               1000
#define ITEM_NUM          22
#define WEIGHT_LIMIT      100
#define CROSSOVER_RATE    0.5
#define MUTATION_RATE     0.01
#define REPRODUCTION_RATE 0.01
#define MAX_ITTER         1000

typedef struct {
	sbs_bitset c;
	int fitness, weight, value;
} chromosome;

typedef struct {
	char *name;
	int weight;
	int value;
} Item;

void init_generation(chromosome * gen);
void make_new_generation(chromosome * gen,
			 chromosome * next_gen, Item * item_list);
void make_first_generation(chromosome * gen, Item * item_list);

void init_chrom(chromosome * a, size_t s);
void make_chrom_zero(chromosome * chrom);
void cleanup_chrom(chromosome * a);
void cleanup_generation(chromosome * gen);

void calculate_fitness(chromosome * gen, Item * item_list);
void mutate(chromosome * out);
void crossover(chromosome a, chromosome b,
	       chromosome * out1, chromosome * out2);
void move_to_next_generation(chromosome a, chromosome * out);
void copy_generation(chromosome * dst, chromosome * src);
void copy_chrom(chromosome * dst, chromosome * src);

void out(chromosome * gen);
void get_best(chromosome * gen, chromosome * best);
void out_best(chromosome * gen, Item * item_list);
void outb(chromosome * a);

#endif				/* __SGA__ */
