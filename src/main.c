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
    error_choice:
        printf("1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Database\n");
        printf("4. Save Database\n");
        printf("5. Update Database\n");
        printf("6. Exit\n");
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
            case 4: {
                printf("Enter the file name you want to save: ");
                char file_name[WORD_SIZE];
                scanf("%s",file_name);
                save_database(hash_table_array,file_name);
                break;
            }
            case 5: {
                printf("Enter the file name: ");
                char file_name[WORD_SIZE];
                scanf("%s",file_name);
                update_database(hash_table_array,file_name);
                break;
            }

            case 6: {
                free_database(hash_table_array);
                exit(EXIT_SUCCESS);
                break;
            }

            default:{
                printf("[ERROR] Enter an valid choice\n");
                goto error_choice;
            }
        }

    } while (1);

}
