#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "ga.h"

static int
check_functions(struct sga_ga *ga)
{
	if (ga->select_parent == NULL) {
		fprintf(stderr, "select_parent function is not provided\n");
		return 1;
	}
	if (ga->fitness_func == NULL) {
		fprintf(stderr, "fitness_func function is not provided\n");
		return 1;
	}
	if (ga->mutate == NULL) {
		fprintf(stderr, "mutate function is not provided\n");
		return 1;
	}
	if (ga->crossover == NULL) {
		fprintf(stderr, "crossover function is not provided\n");
		return 1;
	}
	return 0;
}

struct sga_ga *
sga_new_ga()
{
	struct sga_ga *ga;
	ga = (struct sga_ga *)malloc(sizeof(struct sga_ga));
	ga->num_population = 1000;
	ga->num_generation = 100;
	ga->num_parents = 2;
	ga->individual_size = 0;
	ga->init_best = 0;
	ga->crossover_rate = 0.5;
	ga->mutation_rate = 0.01;
	ga->reproduction_rate = 0.01;
	ga->rand = sga_rand;
	ga->select_parent = sga_select_parent;
	ga->fitness_func = NULL;
	ga->mutate = sga_mutate;
	ga->crossover = sga_crossover;

	return ga;
}

int
sga_run(struct sga_ga *ga)
{
	if (check_functions(ga) != 0)
		return 1;

	srand(time(NULL));
	sga_init_individual(&ga->best, ga->individual_size);
	ga->init_best = 1;

	struct sga_generation gen1, gen2;
	sga_init_generation(ga, &gen1);
	sga_init_generation(ga, &gen2);

	sga_first_generation(ga, &gen1);

	sga_set_best(ga, &gen1);
	uint32_t i;
	for (i = 0; i < ga->num_generation; i++) {
		if ((i & 1) == 0) {
			sga_new_generation(ga, &gen1, &gen2);
			sga_set_best(ga, &gen2);
		} else {
			sga_new_generation(ga, &gen2, &gen1);
			sga_set_best(ga, &gen1);
		}
	}

	sga_delete_generation(&gen1);
	sga_delete_generation(&gen2);

	return 0;
}

void
sga_first_generation(struct sga_ga *ga, struct sga_generation *gen)
{
	uint32_t i, j;

	for (i = 0; i < gen->num_population; i++) {
		for (j = 0; j < gen->individual_size; j++) {
			gen->individuals[i].chrom[j] = ga->rand();
		}
		gen->individuals[i].fitness =
		    ga->fitness_func(gen, &gen->individuals[i], i);
	}
}

void
sga_new_generation(struct sga_ga *ga,
		   struct sga_generation *gen1, struct sga_generation *gen2)
{
	uint32_t i;
	struct sga_individual *parent1, *parent2;

	for (i = 0; i < gen2->num_population; i++) {
		ga->select_parent(gen1, &parent1, &parent2);

		if (RAND01() < ga->reproduction_rate) {
			sga_copy_individual(&gen2->individuals[i],
					    parent1, gen2->individual_size);
		} else if (RAND01() < ga->crossover_rate) {
			ga->crossover(gen2, parent1, parent2,
				      &gen2->individuals[i]);
		}
		if (RAND01() < ga->mutation_rate) {
			ga->mutate(gen2, &gen2->individuals[i]);
		}
		gen2->individuals[i].fitness =
		    ga->fitness_func(gen2, &gen2->individuals[i], i);
	}
}

void
sga_set_best(struct sga_ga *ga, struct sga_generation *gen)
{
	uint32_t i;
	struct sga_individual *max = &gen->individuals[0];
	for (i = 1; i < gen->num_population; i++) {
		if (gen->individuals[i].fitness > max->fitness)
			max = &gen->individuals[i];
	}
	if (max->fitness > ga->best.fitness) {
		sga_copy_individual(&ga->best, max, gen->individual_size);
	}
}

void
sga_init_individual(struct sga_individual *a, uint32_t s)
{
	a->chrom = (double *)calloc(s, sizeof(double));
	a->fitness = 0;
}

void
sga_init_generation(struct sga_ga *ga, struct sga_generation *gen)
{
	uint32_t i;

	gen->num_population = ga->num_population;
	gen->individual_size = ga->individual_size;
	gen->individuals = (struct sga_individual *)
	    malloc(gen->num_population * sizeof(struct sga_individual));
	for (i = 0; i < gen->num_population; i++) {
		sga_init_individual(&gen->individuals[i], gen->individual_size);
	}
}

void
sga_copy_individual(struct sga_individual *dst,
		    struct sga_individual *src, uint32_t s)
{
	memcpy(dst->chrom, src->chrom, s * sizeof(double));
	dst->fitness = src->fitness;
}

void
sga_copy_generation(struct sga_generation *dst, struct sga_generation *src)
{
	uint32_t i;
	for (i = 0; i < dst->num_population; i++) {
		sga_copy_individual(&dst->individuals[i],
				    &src->individuals[i], dst->individual_size);
	}
}

void
sga_delete_individual(struct sga_individual *a)
{
	free(a->chrom);
}

void
sga_delete_generation(struct sga_generation *gen)
{
	uint32_t i;
	for (i = 0; i < gen->num_population; i++) {
		sga_delete_individual(&gen->individuals[i]);
	}
	free(gen->individuals);
}

void
sga_delete_ga(struct sga_ga *ga)
{
	if (ga->init_best)
		free(ga->best.chrom);
	free(ga);
}

double
sga_rand(void)
{
	return (double)rand();
}

void
sga_select_parent(struct sga_generation *gen,
		  struct sga_individual **p1, struct sga_individual **p2)
{
	int r1, r2;

	DIFFRAND(r1, r2, gen->num_population);
	*p1 = &gen->individuals[r1];
	*p2 = &gen->individuals[r2];
}

void
sga_mutate(struct sga_generation *gen, struct sga_individual *a)
{
	int r1, r2;
	double tmp;

	DIFFRAND(r1, r2, gen->individual_size);
	tmp = a->chrom[r1];
	a->chrom[r1] = a->chrom[r2];
	a->chrom[r2] = tmp;
}

void
sga_crossover(struct sga_generation *gen,
	      struct sga_individual *p1, struct sga_individual *p2,
	      struct sga_individual *child)
{
	int point, s;

	point = (rand() % (gen->individual_size - 2)) + 1;
	s = sizeof(double);

	memcpy(child->chrom, p1->chrom, point * s);
	memcpy(child->chrom + point, p2->chrom,
	       (gen->individual_size - point) * s);
}
