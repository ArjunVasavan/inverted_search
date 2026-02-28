#include "../include/inverted_search.h"

int main(int argc, char** argv) {

    hash_t hash_table_array[HASH_SIZE];
    file_t* file_list_head = NULL;

    if ( validate_files(argc,argv,&file_list_head) == FAILURE ) {
        printf("[validate_files]: error in validating files \n");
        exit(EXIT_FAILURE);
    }
    create_hash_table(hash_table_array);
    int choice;

    do {

    printf("1. Create Database\n");
    printf("2. Display Database\n");
    printf("3. Search Database\n");
    printf("Enter an choice: ");
    scanf("%d",&choice);
    printf("\n");

    switch (choice) {

        case 1: {

                if ( create_database(hash_table_array,file_list_head) == SUCCESS ) {
                    printf("Database successfully created\n");
                } else {
                    printf("Failure in creating database\n");
                    exit(EXIT_FAILURE);
                }

            break;
        }

        case 2: {
            display_database(hash_table_array);
            break;
        }

        case 3: {

                printf("Enter an Word you want to search: ");
                char word[WORD_SIZE];
                scanf("%s",word);
                search_database(hash_table_array,word);
                break;
            }
    }

    } while (1);

}
