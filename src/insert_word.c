#include "../include/inverted_search.h"

status_t insert_word(hash_t *hash_table, const char *word, const char *file_name) {

    int index = hash_function(word);

    // case 1: bucket is empty
    if ( hash_table[index].head == NULL ) {

        // creating main node
        main_t* new_main_node = malloc(sizeof(main_t));
        new_main_node->file_count = 1;
        new_main_node->next = NULL;
        strcpy(new_main_node->word,word);

        // creating sub_node
        sub_t*  new_sub_node = malloc(sizeof(sub_t));
        new_sub_node->next = NULL;
        strcpy(new_sub_node->file_name,file_name);
        new_sub_node->word_count = 1;

        // linking sub_node to main_node
        new_main_node->sub_link = new_sub_node;

        // attatching main_node to bucket
        hash_table[index].head = new_main_node;
        return SUCCESS;
    }

    //case 2: bucket is not empty

    main_t* cur_main = hash_table[index].head;
    main_t* prev_main = cur_main;

    while (cur_main) {

        if ( strcmp(cur_main->word,word) == 0 ) {

            //checking is another file is present
            
            sub_t* cur_sub  = cur_main->sub_link;
            sub_t* prev_sub = NULL;

            while (cur_sub) {

                if ( strcmp(cur_sub->file_name,file_name) == 0 ) {
                    cur_sub->word_count+=1;
                    return SUCCESS;
                }
                prev_sub = cur_sub;
                cur_sub = cur_sub->next;
            }
            
            sub_t* new_sub_node = malloc(sizeof(sub_t));
            new_sub_node->word_count = 1;
            strcpy(new_sub_node->file_name,file_name);
            new_sub_node->next = NULL;
            cur_main->file_count+=1;

            prev_sub->next = new_sub_node;

            return SUCCESS;
        }

        prev_main = cur_main;
        cur_main = cur_main->next;
    }


    main_t* new_main_node = malloc(sizeof(main_t));
    new_main_node->next = NULL;
    new_main_node->file_count = 1;
    strcpy(new_main_node->word,word);

    sub_t* new_sub_node = malloc(sizeof(sub_t));
    new_sub_node->next = NULL;
    strcpy(new_sub_node->file_name,file_name);
    new_sub_node->word_count = 1;

    new_main_node->sub_link = new_sub_node;

    prev_main->next = new_main_node;

    return SUCCESS;
}
