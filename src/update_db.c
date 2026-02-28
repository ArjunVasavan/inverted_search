#include "../include/inverted_search.h"
#include <cstddef>

status_t update_database(hash_t *hash_table, const char *file_name) {

    FILE* file_ptr = fopen(file_name,"r");

    if ( file_ptr == NULL ) {
        return FAILURE;
    }

    char line[1024];

    while (fgets(line,sizeof(line),file_ptr)) {

        if ( line[0] != '#' ) continue;

        char* data = line+1; // skips first '#'
        char* token = strtok(data,";");

        int index = atoi(token);
        token = strtok(NULL,";");

        char word[WORD_SIZE];
        strcpy(word,token);

        token = strtok(NULL,";");
        int file_count = atoi(token);

        main_t* new_main_node = malloc(sizeof(main_t));

        if ( new_main_node == NULL ) {
            return FAILURE;
        }

        strcpy(new_main_node->word,word);
        new_main_node->file_count = file_count;
        new_main_node->next = NULL;
        new_main_node->sub_link = NULL;


        for ( int i = 0 ; i < file_count ; i++ ) {

            // first is file name

            token = strtok(NULL,";");
            sub_t* new_sub_node = malloc(sizeof(sub_t));
            if ( new_sub_node == NULL ) {
                return FAILURE;
            }

            strcpy(new_sub_node->file_name,token);

            // second is word count
        
            token = strtok(NULL,";");
            int word_count = atoi(token);
            new_sub_node->word_count = word_count;

            new_sub_node->next = NULL;

            if ( new_main_node->sub_link == NULL ) {
                new_main_node->sub_link = new_sub_node;
            } else {
                sub_t* cur_sub = new_main_node->sub_link;

                while (cur_sub->next) {
                    cur_sub = cur_sub->next;
                }

                cur_sub->next = new_sub_node;

            }


        }

        if (hash_table[index].head == NULL ) {
            hash_table[index].head = new_main_node;
        } else {
            main_t* cur_main = hash_table[index].head;
            while (cur_main->next) {
                cur_main = cur_main->next;
            }
            cur_main->next =new_main_node;
        }

    }

    fclose(file_ptr);
    return SUCCESS;
}
