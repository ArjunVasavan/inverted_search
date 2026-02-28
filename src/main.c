#include "../include/inverted_search.h"

int main(int argc, char** argv) {

    hash_t hash_table_array[HASH_SIZE];
    file_t* file_list_head = NULL;

    if ( validate_files(argc,argv,&file_list_head) == FAILURE ) {
        printf("[validate_files]: error in validating files \n");
        exit(EXIT_FAILURE);
    }
    file_t_print(file_list_head);

    create_hash_table(hash_table_array);

}
