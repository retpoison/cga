#ifndef __SGA__
#define __SGA__

#define RAND01() ((double)rand() / (double)((unsigned)RAND_MAX + 1))

#define DIFFRAND(R1, R2, MAX) \
  do { \
    R1 = rand() % MAX; \
    do { \
      R2 = rand() % MAX; \
    } while (R1 == R2); \
  } while(0)

struct sga_individual {
	double *chrom;
	double fitness;
};

struct sga_generation {
	struct sga_individual *individuals;
	uint32_t individual_size;
	uint32_t num_population;
};

struct sga_ga {
	uint32_t num_population;
	uint32_t num_generation;
	uint32_t num_parents;
	uint32_t individual_size;
	int init_best;
	struct sga_individual best;
	struct sga_individual *last_gen;
	double crossover_rate;
	double mutation_rate;
	double reproduction_rate;

	double (*rand)();
	void (*select_parent)(struct sga_generation * gen,
			      struct sga_individual ** p1,
			      struct sga_individual ** p2);
	double (*fitness_func)(struct sga_generation * gen,
			       struct sga_individual * indv, int index);

	void (*mutate)(struct sga_generation * gen, struct sga_individual * a);
	void (*crossover)(struct sga_generation * gen,
			  struct sga_individual * p1,
			  struct sga_individual * p2,
			  struct sga_individual * child);

};

struct sga_ga *sga_new_ga();
int sga_run(struct sga_ga *ga);
void sga_first_generation(struct sga_ga *ga, struct sga_generation *gen);
void sga_new_generation(struct sga_ga *ga,
			struct sga_generation *gen1,
			struct sga_generation *gen2);

void sga_set_best(struct sga_ga *ga, struct sga_generation *gen);
void sga_init_individual(struct sga_individual *a, uint32_t s);
void sga_init_generation(struct sga_ga *ga, struct sga_generation *gen);
void sga_copy_individual(struct sga_individual *dst,
			 struct sga_individual *src, uint32_t s);
void sga_copy_generation(struct sga_generation *dst,
			 struct sga_generation *src);

void sga_delete_individual(struct sga_individual *a);
void sga_delete_generation(struct sga_generation *gen);
void sga_delete_ga(struct sga_ga *ga);

double sga_rand(void);
void sga_select_parent(struct sga_generation *gen,
		       struct sga_individual **p1, struct sga_individual **p2);
double sga_fitness(struct sga_generation *gen,
		   struct sga_individual *indv, int index);
void sga_mutate(struct sga_generation *gen, struct sga_individual *a);
void sga_crossover(struct sga_generation *gen,
		   struct sga_individual *p1, struct sga_individual *p2,
		   struct sga_individual *child);

#endif				/* __SGA__ */
