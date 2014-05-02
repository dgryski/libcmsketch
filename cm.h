
#include <stdint.h>

typedef struct {
    int w, d;
    uint32_t **sk;
} cmsketch_t;




void sketch_add(cmsketch_t * cms, unsigned char *b, size_t len, uint32_t count);
cmsketch_t *sketch_clone(cmsketch_t * cms);
void sketch_compress(cmsketch_t * cms);
uint32_t sketch_count(cmsketch_t * cms, unsigned char *b, size_t len);
void sketch_free(cmsketch_t * cms);
void sketch_merge(cmsketch_t * cms1, cmsketch_t * cms2);
cmsketch_t *sketch_new(int w, int d);
uint32_t *sketch_values(cmsketch_t *cms, unsigned char *b, size_t len);
