#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "ga.h"

#define WEIGHT_LIMIT 100
#define UNUSED(X) (void)(X)

struct Item {
	char *name;
	int weight;
	int value;
} *item_list;

double
fitness(struct sga_generation *gen, struct sga_individual *indv, int index)
{
	uint32_t i, w = 0;
	double f = 0;
	UNUSED(index);

	for (i = 0; i < gen->individual_size; i++) {
		if ((int)(indv->chrom[i]) & 1) {
			f += item_list[i].value;
			w += item_list[i].weight;
		}
	}
	if (w > WEIGHT_LIMIT)
		return 0;
	return f;
}

void
select_parent(struct sga_generation *gen,
	      struct sga_individual **p1, struct sga_individual **p2)
{
	int r1, r2, max;

	max = 100;
	do {
		r1 = rand() % gen->num_population;
		*p1 = &gen->individuals[r1];
	} while (gen->individuals[r1].fitness == 0 && max-- > 0);

	max = 100;
	do {
		r2 = rand() % gen->num_population;
		*p2 = &gen->individuals[r2];
	} while (gen->individuals[r2].fitness == 0 && max-- > 0);
}

void
init_item_list()
{
	int i;
	struct Item il[22] = {
		{"map", 9, 150},
		{"compass", 13, 35},
		{"water", 153, 200},
		{"sandwich", 50, 60},
		{"glucose", 15, 60},
		{"tin", 68, 45},
		{"banana", 27, 60},
		{"apple", 39, 40},
		{"cheese", 23, 30},
		{"beer", 52, 10},
		{"suntan cream", 11, 70},
		{"camera", 32, 30},
		{"T-shirt", 24, 15},
		{"trousers", 48, 10},
		{"umbrella", 73, 40},
		{"waterproof trousers", 42, 70},
		{"waterproof overclothes", 43, 75},
		{"note-case", 22, 80},
		{"sunglasses", 7, 20},
		{"towel", 18, 12},
		{"socks", 4, 50},
		{"book", 30, 10},
	};
	item_list = (struct Item *)malloc(22 * sizeof(struct Item));
	for (i = 0; i < 22; i++) {
		item_list[i].name = il[i].name;
		item_list[i].weight = il[i].weight;
		item_list[i].value = il[i].value;
	}
}

int
main()
{
	struct sga_ga *ga;

	init_item_list();
	ga = sga_new_ga();
	ga->fitness_func = fitness;
	ga->select_parent = select_parent;
	ga->individual_size = 22;
	if (sga_run(ga) != 0)
		goto err;

	uint32_t i, j = 0;
	printf("num\t\tname\t\tweight\t\tvalue\n");
	for (i = 0; i < ga->individual_size; i++) {
		if ((int)(ga->best.chrom[i]) & 1) {
			printf("%d\t%-20s\t%d\t\t%d\n", j, item_list[i].name,
			       item_list[i].weight, item_list[i].value);
			j++;
		}
	}
	printf("fitness: %lf\n", ga->best.fitness);

 err:
	sga_delete_ga(ga);
	free(item_list);
	return 0;
}
