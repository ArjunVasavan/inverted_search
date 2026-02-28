#include "../include/inverted_search.h"

status_t validate_files(int argc, char **argv, file_t **head) {

    *head = NULL;

    if ( argc <= 1 ) {
        return FAILURE;
    }

    for ( int i = 1 ; i < argc ; i++ ) {

        // FIXME: it wont solve arjun.txt123 issue
        if ( strstr(argv[i],".txt") == 0 ) {
            continue;
        }

        FILE* check_file = fopen(argv[i],"r");
        if ( check_file == NULL ) {
            printf("Cannot open %s\n",argv[i]);
            // what i learned here is if you try to close this its seg fault because its NULL
            continue;
        }

        fclose(check_file);

        file_t* new_node = malloc(sizeof(file_t));

        if ( new_node == NULL ) {
            return FAILURE;
        }

        strcpy(new_node->file_name,argv[i]);

        new_node->next = NULL;

        if ( *head == NULL ) {
            *head = new_node;

        } else {

            file_t* temp = *head;

            while (temp->next) {
                temp = temp->next;
            }

            temp->next = new_node;
        }

    }

    if ( *head == NULL ) {
        return FAILURE;
    }

    return SUCCESS;
}
