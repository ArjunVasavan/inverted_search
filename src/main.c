#include "../include/inverted_search.h"

int main(int argc, char **argv)
{
    hash_t hash_table_array[HASH_SIZE];
    file_t *file_list_head = NULL;

    db_state_t state = DB_EMPTY;

    if (validate_files(argc, argv, &file_list_head) == FAILURE) {
        printf("File validation failed\n");
        return FAILURE;
    }

    create_hash_table(hash_table_array);

    int choice;

    while (1) {

        printf("\n1. Create Database\n");
        printf("2. Display Database\n");
        printf("3. Search Database\n");
        printf("4. Save Database\n");
        printf("5. Update Database\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1:
                if (state == DB_EMPTY || state == DB_UPDATED) {

                    if (create_database(hash_table_array, file_list_head) == SUCCESS) {
                        printf("Database created successfully\n");
                        state = DB_CREATED;
                    }

                } else {
                    printf("[ERROR]: Create not allowed\n");
                }
                break;

            case 2:
                display_database(hash_table_array);
                break;

            case 3: {
                char word[WORD_SIZE];
                printf("Enter word: ");
                scanf("%s", word);
                search_database(hash_table_array, word);
                break;
            }

            case 4: {
                char save_file[WORD_SIZE];
                printf("Enter save file name: ");
                scanf("%s", save_file);
                save_database(hash_table_array, save_file);
                break;
            }

            case 5:
                if (state == DB_EMPTY) {

                    char backup_file[WORD_SIZE];
                    printf("Enter backup file name: ");
                    scanf("%s", backup_file);

                    if (update_database(hash_table_array,
                                        backup_file,
                                        &file_list_head) == SUCCESS) {
                        printf("Database updated successfully\n");
                        state = DB_UPDATED;
                    }

                } else {
                    printf("[ERROR]: Update not allowed\n");
                }
                break;

            case 6:
                free_database(hash_table_array);
                return SUCCESS;

            default:
                printf("Invalid choice\n");
        }
    }
}
