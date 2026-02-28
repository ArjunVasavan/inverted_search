#include "../include/inverted_search.h"

int main(int argc, char** argv) {

    hash_t* hash_table_array;
    file_t* file_list_head = NULL;

    if ( validate_files(argc,argv,&file_list_head) == FAILURE ) {
        printf("[validate_files]: error in validating files \n");
        exit(EXIT_FAILURE);
    }

}
