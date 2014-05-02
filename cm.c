

#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cm.h"

/* prototypes for our hash functions */
static uint32_t leveldb_bloom_hash(unsigned char *b, size_t len);
static uint32_t jenkins_hash(unsigned char *b, size_t len);

/* allocate a new sketch  with width w and depth d */
cmsketch_t *sketch_new(int w, int d)
{

    cmsketch_t *cms = calloc(1, sizeof(cmsketch_t));

    cms->w = w;
    cms->d = d;

    cms->sk = calloc(d, sizeof(uint32_t *));
    for (int i = 0; i < d; i++) {
	cms->sk[i] = calloc(w, sizeof(uint32_t));
    }

    return cms;
}

void sketch_free(cmsketch_t * cms)
{

    for (int i = 0; i < cms->d; i++) {
	free(cms->sk[i]);
    }

    free(cms->sk);

    free(cms);
}

void sketch_add(cmsketch_t * cms, unsigned char *b, size_t len,
		uint32_t count)
{

    uint32_t h1 = leveldb_bloom_hash(b, len);
    uint32_t h2 = jenkins_hash(b, len);

    for (int i = 0; i < cms->d; i++) {
	int pos = (h1 + i * h2) % cms->w;
	cms->sk[i][pos] += count;
    }
}

uint32_t sketch_count(cmsketch_t * cms, unsigned char *b, size_t len)
{

    uint32_t h1 = leveldb_bloom_hash(b, len);
    uint32_t h2 = jenkins_hash(b, len);

    uint32_t min = UINT32_MAX;

    for (int i = 0; i < cms->d; i++) {
	int pos = (h1 + i * h2) % cms->w;
	if (cms->sk[i][pos] < min) {
	    min = cms->sk[i][pos];
	}
    }

    return min;
}

cmsketch_t *sketch_clone(cmsketch_t * cms)
{

    cmsketch_t *clone = sketch_new(cms->w, cms->d);

    for (int i = 0; i < cms->d; i++) {
	memmove(clone->sk[i], cms->sk[i], cms->w * sizeof(uint32_t));
    }

    return clone;
}

void sketch_merge(cmsketch_t * cms1, cmsketch_t * cms2)
{

    assert(cms1->w == cms2->w);
    assert(cms1->d == cms2->d);

    for (int d = 0; d < cms1->d; d++) {
	for (int w = 0; w < cms1->w; w++) {
	    cms1->sk[d][w] += cms2->sk[d][w];
	}
    }
}

void sketch_compress(cmsketch_t * cms)
{

    assert((cms->w & (cms->w - 1)) == 0);

    /* new width */
    cms->w /= 2;

    for (int d = 0; d < cms->d; d++) {
	uint32_t *row = calloc(cms->w, sizeof(uint32_t));
	for (int w = 0; w < cms->w; w++) {
	    row[w] = cms->sk[d][w] + cms->sk[d][cms->w + w];
	}

	free(cms->sk[d]);
	cms->sk[d] = row;
    }
}

uint32_t *sketch_values(cmsketch_t * cms, unsigned char *b, size_t len)
{

    uint32_t h1 = leveldb_bloom_hash(b, len);
    uint32_t h2 = jenkins_hash(b, len);

    uint32_t *vals = calloc(cms->d, sizeof(uint32_t));

    for (int i = 0; i < cms->d; i++) {
	int pos = (h1 + i * h2) % cms->w;
	vals[i] = cms->sk[i][pos];
    }

    return vals;
}


/* from leveldb, a murmur-lite */
static uint32_t leveldb_bloom_hash(unsigned char *b, size_t len)
{
    const uint32_t seed = 0xbc9f1d34;
    const uint32_t m = 0xc6a4a793;

    uint32_t h = seed ^ len * m;
    while (len >= 4) {
	h += b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
	h *= m;
	h ^= h >> 16;
	b += 4;
	len -= 4;
    }

    switch (len) {
    case 3:
	h += b[2] << 16;
    case 2:
	h += b[1] << 8;
    case 1:
	h += b[0];
	h *= m;
	h ^= h >> 24;
    }
    return h;
}

static uint32_t jenkins_hash(unsigned char *b, size_t len)
{

    uint32_t h2 = 0;

    for (int i = 0; i < len; i++) {
	h2 += b[i];
	h2 += h2 << 10;
	h2 ^= h2 >> 6;
    }
    h2 += (h2 << 3);
    h2 ^= (h2 >> 11);
    h2 += (h2 << 15);

    return h2;
}
