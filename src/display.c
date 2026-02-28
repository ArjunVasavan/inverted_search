#include "../include/inverted_search.h"
#include <stdio.h>

void display_database(hash_t *hash_table) {

    for ( int i = 0 ; i < HASH_SIZE ; i++ ) {

        if ( hash_table[i].head == NULL ) {
            continue;
        }

        printf("Index: %d\n",i);

        main_t* cur_main = hash_table[i].head;

        while (cur_main) {
            printf("Word : %s ( %d files )\n",cur_main->word,cur_main->file_count);
            sub_t* cur_sub = cur_main->sub_link;
            while (cur_sub) {
                printf("       %s --> %d \n",cur_sub->file_name,cur_sub->word_count);
                cur_sub = cur_sub->next;
            }
            cur_main = cur_main->next;
        }
        printf("\n");
    }
}
