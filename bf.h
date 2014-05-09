
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int w, h;
    uint64_t *bits;
} bfilter_t;

void bfilter_add(bfilter_t * bf, unsigned char *b, size_t len);
bfilter_t *bfilter_clone(bfilter_t * bf);
void bfilter_compress(bfilter_t * bf);
int bfilter_exists(bfilter_t * bf, unsigned char *b, size_t len);
void bfilter_free(bfilter_t * bf);
void bfilter_merge(bfilter_t * bf1, bfilter_t * bf2);
bfilter_t *bfilter_new(int w, int h);
