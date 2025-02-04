#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "ga.h"
#define SBS_BITSET_IMPLEMENTATION
#include "bitset.h"

void
init_generation(chromosome * gen)
{
	size_t s = (ITEM_NUM / sbs_typelen) + 1;
	int i;
	for (i = 0; i < POP; i++) {
		init_chrom(&gen[i], s);
	}
}

void
make_new_generation(chromosome * gen, chromosome * next_gen, Item * item_list)
{
	int i, r1, r2;
	int p = ((POP & 1) == 1) ? POP - 1 : POP;
	for (i = 0; i < p; i += 2) {
		make_chrom_zero(&next_gen[i]);
		make_chrom_zero(&next_gen[i + 1]);
		r1 = rand() % POP;
		r2 = rand() % POP;
		if (RAND01 < REPRODUCTION_RATE) {
			move_to_next_generation(gen[r1], &next_gen[i]);
			move_to_next_generation(gen[r2], &next_gen[i + 1]);
		} else if (RAND01 < CROSSOVER_RATE) {
			crossover(gen[r1], gen[r2], &next_gen[i],
				  &next_gen[i + 1]);
		}
		if (RAND01 < MUTATION_RATE) {
			mutate(&next_gen[i]);
			mutate(&next_gen[i + 1]);
		}
		calculate_fitness(&next_gen[i], item_list);
		calculate_fitness(&next_gen[i + 1], item_list);
	}

	for (; i < POP; i++)
		move_to_next_generation(gen[i], &next_gen[i]);
}

void
make_first_generation(chromosome * gen, Item * item_list)
{
	int i, j;
	for (i = 0; i < POP; i++) {
		for (j = 0; j < ITEM_NUM; j++) {
			if (rand() & 1)
				sbs_set_bit(&gen[i].c, j);
			else
				sbs_clr_bit(&gen[i].c, j);
		}
		calculate_fitness(&gen[i], item_list);
	}
}

void
init_chrom(chromosome * a, size_t s)
{
	sbs_set_size(&a->c, s);
	sbs_init_bitset(&a->c);
	make_chrom_zero(a);
}

void
make_chrom_zero(chromosome * chrom)
{
	chrom->fitness = 0;
	chrom->weight = 0;
	chrom->value = 0;
}

void
cleanup_chrom(chromosome * a)
{
	sbs_free_bitset(&a->c);
}

void
cleanup_generation(chromosome * gen)
{
	int i;
	for (i = 0; i < POP; i++) {
		cleanup_chrom(&gen[i]);
	}
	gen = NULL;
}

void
calculate_fitness(chromosome * gen, Item * item_list)
{
	int i;
	for (i = 0; i < ITEM_NUM; i++) {
		if (!(sbs_get_bit(&gen->c, i)))
			continue;
		gen->value += item_list[i].value;
		gen->weight += item_list[i].weight;
	}
	if (gen->weight <= WEIGHT_LIMIT)
		gen->fitness = gen->value;
}

void
mutate(chromosome * out)
{
	int r = rand() % ITEM_NUM;
	if (sbs_get_bit(&out->c, r))
		sbs_clr_bit(&out->c, r);
	else
		sbs_set_bit(&out->c, r);
}

void
crossover(chromosome a, chromosome b, chromosome * out1, chromosome * out2)
{
	int point = (rand() % (ITEM_NUM - 2)) + 1;
	int i;
	for (i = 0; i < ITEM_NUM - point; i++) {
		if (sbs_get_bit(&a.c, i))
			sbs_set_bit(&out1->c, i);
		else
			sbs_clr_bit(&out1->c, i);

		if (sbs_get_bit(&b.c, i))
			sbs_set_bit(&out2->c, i);
		else
			sbs_clr_bit(&out2->c, i);
	}
	for (; i < ITEM_NUM; i++) {
		if (sbs_get_bit(&b.c, i))
			sbs_set_bit(&out1->c, i);
		else
			sbs_clr_bit(&out1->c, i);

		if (sbs_get_bit(&a.c, i))
			sbs_set_bit(&out2->c, i);
		else
			sbs_clr_bit(&out2->c, i);
	}
}

void
move_to_next_generation(chromosome a, chromosome * out)
{
	copy_chrom(&a, out);
}

void
copy_generation(chromosome * dst, chromosome * src)
{
	int i;
	for (i = 0; i < POP; i++) {
		copy_chrom(&dst[i], &src[i]);
	}
}

void
copy_chrom(chromosome * dst, chromosome * src)
{
	memcpy(dst->c.bits, src->c.bits, sizeof(dst->c.size) * dst->c.size);
	dst->fitness = src->fitness;
	dst->weight = src->weight;
	dst->value = src->value;
}

void
out(chromosome * gen)
{
	int i, j, max = 0;
	for (i = 0; i < POP; i++) {
		for (j = 0; j < ITEM_NUM; j++) {
			printf("%d", sbs_get_bit(&gen[i].c, j));
		}
		if (gen[i].fitness > max)
			max = gen[i].fitness;
		printf(" f%d p%d w%d\n", gen[i].fitness,
		       gen[i].value, gen[i].weight);
	}
	printf("max: %d\n", max);
}

void
get_best(chromosome * gen, chromosome * best)
{
	int i;
	chromosome *max = &gen[0];
	for (i = 1; i < POP; i++) {
		if (gen[i].fitness > max->fitness)
			max = &gen[i];
	}
	if (max->fitness > best->fitness)
		copy_chrom(best, max);
}

void
out_best(chromosome * ch, Item * item_list)
{
	int i, j = 1;
	printf("num\t\tname\t\tweight\t\tvalue\n");
	for (i = 0; i < ITEM_NUM; i++) {
		if (sbs_get_bit(&ch->c, i)) {
			printf("%d\t%-20s\t%d\t\t%d\n", j, item_list[i].name,
			       item_list[i].weight, item_list[i].value);
			j++;
		}
	}
//  outb(ch);
	printf("chromosome:\n");
	printf("fitness: %d\nvalues: %d\nweight:%d\n",
	       ch->fitness, ch->value, ch->weight);
}

void
outb(chromosome * a)
{
	size_t i;
	for (i = 0; i < sbs_typelen; i++) {
		printf("%d", sbs_get_bit(&a->c, i));
	}
	printf("\n");
}
