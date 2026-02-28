#include "../include/inverted_search.h"

void free_database(hash_t *hash_table) {

    for ( int i = 0 ; i < HASH_SIZE ; i++ ) {

        if ( hash_table[i].head == NULL ) {
            continue;
        }

        main_t* cur_main = hash_table[i].head;
        main_t*  prev_main = NULL;

        while (cur_main) {

            sub_t* prev_sub = NULL;
            sub_t* cur_sub = cur_main->sub_link;

            while (cur_sub) {

                prev_sub = cur_sub;
                cur_sub = cur_sub->next;
                free(prev_sub);
            }

            prev_main = cur_main;
            cur_main = cur_main->next;
            free(prev_main);
        }
        hash_table[i].head = NULL;
    }

}

