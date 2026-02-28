#include "../include/inverted_search.h"

void file_t_print(file_t* head) {
    printf("\n");
    file_t* temp = head;
    while (temp) {
        printf("%s",temp->file_name);
        printf("->");
        if ( temp->next == NULL ) {
            break;
        }
        temp = temp->next;
    }
    printf("\n");
}
