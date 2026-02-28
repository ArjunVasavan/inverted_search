#include "../include/inverted_search.h"

void remove_file_from_list(file_t **head, const char *file_name) {
    file_t *cur = *head;
    file_t *prev = NULL;

    while (cur) {
        if (strcmp(cur->file_name, file_name) == 0) {

            if (prev) {
                prev->next = cur->next;
            } else {
                *head = cur->next;
            }

            free(cur);
            return;
        }

        prev = cur;
        cur = cur->next;
    }
}

status_t update_database(hash_t hash_table[],
                         const char *file_name,
                         file_t **file_list_head)
{
    FILE *file_ptr = fopen(file_name, "r");
    if (!file_ptr) {
        return FAILURE;
    }

    char line[1024];

    while (fgets(line, sizeof(line), file_ptr)) {

        if (line[0] != '#')
            continue;

        char *data = line + 1;
        char *token = strtok(data, ";");

        int index = atoi(token);

        token = strtok(NULL, ";");
        char word[WORD_SIZE];
        strcpy(word, token);

        token = strtok(NULL, ";");
        int file_count = atoi(token);

        main_t *new_main = malloc(sizeof(main_t));
        if (!new_main) {
            fclose(file_ptr);
            return FAILURE;
        }

        strcpy(new_main->word, word);
        new_main->file_count = file_count;
        new_main->next = NULL;
        new_main->sub_link = NULL;

        for (int i = 0; i < file_count; i++) {

            token = strtok(NULL, ";");

            sub_t *new_sub = malloc(sizeof(sub_t));
            if (!new_sub) {
                fclose(file_ptr);
                return FAILURE;
            }

            strcpy(new_sub->file_name, token);

            remove_file_from_list(file_list_head, token);

            token = strtok(NULL, ";");
            new_sub->word_count = atoi(token);
            new_sub->next = NULL;

            if (!new_main->sub_link) {
                new_main->sub_link = new_sub;
            } else {
                sub_t *cur = new_main->sub_link;
                while (cur->next)
                    cur = cur->next;
                cur->next = new_sub;
            }
        }

        if (!hash_table[index].head) {
            hash_table[index].head = new_main;
        } else {
            main_t *cur = hash_table[index].head;
            while (cur->next)
                cur = cur->next;
            cur->next = new_main;
        }
    }

    fclose(file_ptr);
    return SUCCESS;
}
