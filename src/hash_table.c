#include "../include/inverted_search.h"

void create_hash_table(hash_t *hash_table) {

    for ( int i = 0 ; i < HASH_SIZE ; i++ ) {
        hash_table[i].index = i;
        hash_table[i].head = NULL;
    }
}

int hash_function(const char *word) {

    if ( word == NULL ) {
        return 26;
    }

    char first_character = word[0];

    if ( first_character >= 'a' && first_character <= 'z' ) {
        return first_character - 'a' ;
    } else if ( first_character >= 'A' && first_character <= 'Z' ) {
        first_character = tolower(first_character);
        return first_character - 'a' ;
    }

    return 26;
}
