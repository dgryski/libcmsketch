
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cm.h"
#include "bf.h"

#define MAXLINELEN  256

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("usage: %s testdata/domains.txt query\n", argv[0]);
        exit(1);
    }

    unsigned char line[MAXLINELEN];
    FILE *f = fopen(argv[1], "r");
    cmsketch_t *cms = sketch_new(1 << 20, 4);
    bfilter_t *bf = bfilter_new(1<<20, 4);

    while(fgets((char *)line, MAXLINELEN, f)) {
        size_t len = strlen((char *)line);
        len--;
        line[len] = '\0';
        sketch_add(cms, line, len, 1);
        bfilter_add(bf, line, len);
    }

    unsigned char *query = (unsigned char *)argv[2];
    size_t querylen = strlen((char *)query);

    printf("%s? %s\n", query, bfilter_exists(bf, query, querylen) ? "true" : "false");
    printf("%s: %d\n", query, sketch_count(cms, query, querylen));

    uint32_t *vals = sketch_values(cms, query, querylen);

    printf("%s:", query);
    for(int i=0;i<cms->d;i++) {
        printf(" %d", vals[i]);
    }
    printf("\n");
    free(vals);

    cmsketch_t *clone = sketch_clone(cms);

    printf("%s: %d\n", query, sketch_count(clone, query, querylen));

    sketch_merge(cms, clone);

    printf("%s: %d\n", query, sketch_count(cms, query, querylen));

    printf("%s: %d\n", query, sketch_count(cms, query, querylen));

    sketch_free(cms);
    sketch_free(clone);
}
