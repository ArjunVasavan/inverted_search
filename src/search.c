#include "../include/inverted_search.h"

status_t search_database(hash_t *hash_table, const char *word) {

    int index = hash_function(word);

    if (hash_table[index].head == NULL) {
        return DATA_NOT_FOUND;
    }

    main_t* cur_main = hash_table[index].head;

    while (cur_main) {

        if (strcmp(cur_main->word,word) == 0 ) {
            break;
        }

        cur_main = cur_main->next;
    }

    if ( cur_main == NULL ) {
        return DATA_NOT_FOUND;
    }

    sub_t* cur_sub = cur_main->sub_link;

    printf("\n--------------------------------------------------\n");
    printf("Word: %s\n", word);
    printf("Total Files: %d\n\n", cur_main->file_count);

    printf("%-20s %s\n", "File Name", "Occurrences");
    printf("----------------------------------------\n");

    while (cur_sub)
    {
        printf("%-20s %d\n",
               cur_sub->file_name,
               cur_sub->word_count);

        cur_sub = cur_sub->next;
    }

    printf("--------------------------------------------------\n\n");

    return SUCCESS;
}
