
#include <stdint.h>
#include <stddef.h>

/* from leveldb, a murmur-lite */
uint32_t leveldb_bloom_hash(unsigned char *b, size_t len)
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

uint32_t jenkins_hash(unsigned char *b, size_t len)
{

    uint32_t h2 = 0;

    for (unsigned int i = 0; i < len; i++) {
	h2 += b[i];
	h2 += h2 << 10;
	h2 ^= h2 >> 6;
    }
    h2 += (h2 << 3);
    h2 ^= (h2 >> 11);
    h2 += (h2 << 15);

    return h2;
}
