#ifndef INV_H
#define INV_H

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SUCCESS,
    FAILURE
} status;

#define HASH_SIZE 27

// this is to store filename and word count

typedef struct sub_node_t {
    char file_name[100];
    int word_count;
    struct sub_node_t* link;
} sub_node_t;

typedef struct main_node_t {
    int file_count;
    char word[100];
    sub_node_t* sub_link;
    struct main_node_t* main_link;
} main_node_t;

typedef struct hash_t {
    main_node_t* link;
} hash_t;

typedef struct file_val_t {
    char file_name[100];
    struct file_val_t* link;
} file_val_t;


#endif 
