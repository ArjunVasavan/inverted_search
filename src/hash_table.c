#include "../include/inverted_search.h"

void create_hash_table(hash_t *hash_table) {

    for ( int i = 0 ; i < HASH_SIZE ; i++ ) {
        hash_table[i].index = i;
        hash_table[i].head = NULL;
    }
}
