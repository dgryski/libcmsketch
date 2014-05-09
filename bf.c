
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "bf.h"
#include "hash.h"

enum { BITSPERWORD = 64 };
static int bit_test(uint64_t * bits, unsigned int bit)
{
    return bits[bit / BITSPERWORD] & (1 << bit % BITSPERWORD);
}

static void bit_set(uint64_t * bits, unsigned int bit)
{
    bits[bit / BITSPERWORD] |= (1 << bit % BITSPERWORD);
}

/* allocate a new bloom filter  with width w (in bits) and h hash functions */
bfilter_t *bfilter_new(int w, int h)
{

    bfilter_t *bf = calloc(1, sizeof(bfilter_t));

    bf->w = w;
    bf->h = h;

    int words = (w+BITSPERWORD-1)/BITSPERWORD;

    bf->bits = calloc(words, sizeof(uint64_t));

    return bf;
}

void bfilter_free(bfilter_t * bf)
{

    free(bf->bits);

    free(bf);
}

void bfilter_add(bfilter_t * bf, unsigned char *b, size_t len)
{

    uint32_t h1 = leveldb_bloom_hash(b, len);
    uint32_t h2 = jenkins_hash(b, len);

    for (int i = 0; i < bf->h; i++) {
	int pos = (h1 + i * h2) % bf->w;
	bit_set(bf->bits, pos);
    }
}

int bfilter_exists(bfilter_t * bf, unsigned char *b, size_t len)
{

    uint32_t h1 = leveldb_bloom_hash(b, len);
    uint32_t h2 = jenkins_hash(b, len);

    for (int i = 0; i < bf->h; i++) {
	int pos = (h1 + i * h2) % bf->w;
	if (!bit_test(bf->bits, pos)) {
	    return 0;
	}
    }

    return 1;
}

bfilter_t *bfilter_clone(bfilter_t * bf)
{

    bfilter_t *clone = bfilter_new(bf->w, bf->h);

    memmove(clone->bits, bf->bits, bf->w * sizeof(uint64_t));

    return clone;
}

void bfilter_merge(bfilter_t * bf1, bfilter_t * bf2)
{

    assert(bf1->w == bf2->w);
    assert(bf1->h == bf2->h);

    for (int i = 0; i < bf1->w; i++) {
	bf1->bits[i] |= bf2->bits[i];
    }
}

void bfilter_compress(bfilter_t * bf)
{

    assert((bf->w & (bf->w - 1)) == 0);

    /* new width */
    bf->w /= 2;

    uint64_t *bits = calloc(bf->w, sizeof(uint64_t));
    for (int i = 0; i < bf->w; i++) {
	bits[i] = bf->bits[i] | bf->bits[bf->w + i];
    }

    free(bf->bits);
    bf->bits = bits;
}
