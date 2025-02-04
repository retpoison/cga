#ifndef SBS_BITSET_H
#define SBS_BITSET_H

#include <stdint.h>
#include <limits.h>

#ifndef SBSSTDEF
#define SBSSTDEF extern
#endif

#define sbs_settype uint64_t
#define sbs_typelen (sizeof(sbs_settype) * CHAR_BIT)

#define sbs_check_bitset(bst) do {                   \
	sbs_tmp_var = pos / sbs_typelen;                   \
	if (bst->bits == NULL || sbs_tmp_var >= bst->size) \
		return -1;                                       \
} while (0)

typedef struct {
	sbs_settype *bits;
	size_t size;
} sbs_bitset;

SBSSTDEF size_t sbs_tmp_var;
SBSSTDEF sbs_settype sbs_setbit[sbs_typelen], sbs_clrbit[sbs_typelen];

SBSSTDEF void sbs_init_bitset(sbs_bitset * bst);
SBSSTDEF void sbs_init_array(void);
SBSSTDEF void sbs_free_bitset(sbs_bitset * bst);
SBSSTDEF void sbs_set_size(sbs_bitset * bst, size_t s);
SBSSTDEF int sbs_set_bit(sbs_bitset * bst, size_t pos);
SBSSTDEF int sbs_clr_bit(sbs_bitset * bst, size_t pos);
SBSSTDEF int sbs_get_bit(sbs_bitset * bst, size_t pos);

#endif				/* SBS_BITSET_H */

#ifdef SBS_BITSET_IMPLEMENTATION
#ifndef SBS_BITSET_C
#define SBS_BITSET_C

size_t sbs_tmp_var;
sbs_settype sbs_setbit[sbs_typelen] = { 0 }, sbs_clrbit[sbs_typelen] = { 0 };

SBSSTDEF void
sbs_init_bitset(sbs_bitset * bst)
{
	bst->bits = (sbs_settype *) calloc(bst->size, sbs_typelen);
}

SBSSTDEF void
sbs_init_array(void)
{
	sbs_settype i;
	for (i = 0; i < sbs_typelen; i++) {
		sbs_setbit[i] = (sbs_settype) 1 << i;
		sbs_clrbit[i] = ~sbs_setbit[i];
	}
}

SBSSTDEF void
sbs_free_bitset(sbs_bitset * bst)
{
	if (bst->bits != NULL)
		free(bst->bits);
	bst->bits = NULL;
}

SBSSTDEF void
sbs_set_size(sbs_bitset * bst, size_t s)
{
	bst->size = s;
}

SBSSTDEF int
sbs_set_bit(sbs_bitset * bst, size_t pos)
{
	sbs_check_bitset(bst);
	bst->bits[sbs_tmp_var] |= sbs_setbit[pos % sbs_typelen];
	return 0;
}

SBSSTDEF int
sbs_clr_bit(sbs_bitset * bst, size_t pos)
{
	sbs_check_bitset(bst);
	bst->bits[sbs_tmp_var] &= sbs_clrbit[pos % sbs_typelen];
	return 0;
}

SBSSTDEF int
sbs_get_bit(sbs_bitset * bst, size_t pos)
{
	sbs_check_bitset(bst);
	return (bst->bits[sbs_tmp_var] >> (pos % sbs_typelen)) & 1;
}

#endif				/* SBS_BITSET_C */
#endif				/* SBS_BITSET_IMPLEMENTATION */
