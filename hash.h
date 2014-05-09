
#include <stddef.h>

uint32_t leveldb_bloom_hash(unsigned char *b, size_t len);
uint32_t jenkins_hash(unsigned char *b, size_t len);
