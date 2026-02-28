#include "../include/inverted_search.h"

status_t save_database(hash_t *hash_table, const char *file_name) {

    FILE* file_ptr = fopen(file_name,"w");

    if ( file_ptr == NULL ) {
        return FAILURE;
    }

    for ( int i = 0 ; i < HASH_SIZE ; i++ ) {

        if ( hash_table[i].head == NULL ) {
            continue;
        }

        main_t* cur_main = hash_table[i].head;

        while (cur_main) {
        
            fprintf(file_ptr,"#%d;%s;%d;",i,cur_main->word,cur_main->file_count);

            sub_t* cur_sub = cur_main->sub_link;

            while (cur_sub) {
                fprintf(file_ptr,"%s;%d;",cur_sub->file_name,cur_sub->word_count);
                cur_sub = cur_sub->next;
            }
            
            fprintf(file_ptr,"#\n");

            cur_main = cur_main->next;
        }

    }

    fclose(file_ptr);

    return SUCCESS;
}
