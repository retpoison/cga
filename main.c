#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include "ga.h"
#include "bitset.h"

void
init_item_list(Item * item_list)
{
	Item il[22] = {
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
	int i;
	for (i = 0; i < ITEM_NUM; i++) {
		item_list[i].name = il[i].name;
		item_list[i].weight = il[i].weight;
		item_list[i].value = il[i].value;
	}
}

int
main()
{
	srand(time(NULL));
	sbs_init_array();

	Item *item_list = (Item *) malloc(ITEM_NUM * sizeof(Item));
	init_item_list(item_list);

	chromosome gen[POP], next_gen[POP], best;
	init_generation(gen);
	init_generation(next_gen);
	init_chrom(&best, (ITEM_NUM / sbs_typelen) + 1);

	make_first_generation(gen, item_list);

	get_best(gen, &best);
	int i = 0;
	for (; i < MAX_ITTER; i++) {
		if ((i & 1) == 0) {
			make_new_generation(gen, next_gen, item_list);
			get_best(next_gen, &best);
		} else {
			make_new_generation(next_gen, gen, item_list);
			get_best(gen, &best);
		}
	}

	out_best(&best, item_list);
	printf("%d generation%s\n", i, (i > 1) ? "s" : " ");

	cleanup_generation(gen);
	cleanup_generation(next_gen);
	cleanup_chrom(&best);
	free(item_list);
	item_list = NULL;

	return 0;
}
