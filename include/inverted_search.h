#ifndef HASH_H
#define HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_SIZE 27
#define WORD_SIZE 100
#define FILE_NAME_SIZE 100

typedef enum {
    SUCCESS = 0,
    FAILURE = -1,
    LIST_EMPTY = -2,
    DATA_NOT_FOUND = -3
} status_t;


/* File list (input file list)   */

typedef struct file_node {
    char file_name[FILE_NAME_SIZE];
    struct file_node *next;
} file_t;


/* Sub node (file level in DB)   */

typedef struct sub_node {
    char file_name[FILE_NAME_SIZE];
    int word_count;
    struct sub_node *next;
} sub_t;


/* Main node (word level in DB)  */

typedef struct main_node {
    char word[WORD_SIZE];
    int file_count;
    sub_t *sub_link;
    struct main_node *next;
} main_t;


/* Hash table bucket             */

typedef struct hash_node {
    int index;
    main_t *head;
} hash_t;


status_t validate_files(int argc, char *argv[], file_t **head);

void create_hash_table(hash_t hash_table[]);

int hash_function(const char *word);

status_t create_database(hash_t hash_table[], file_t *file_head);

status_t insert_word(hash_t hash_table[],
                     const char *word,
                     const char *file_name);

status_t search_database(hash_t hash_table[],
                         const char *word);

void display_database(hash_t hash_table[]);

status_t save_database(hash_t hash_table[],
                       const char *file_name);

status_t update_database(hash_t hash_table[],
                         const char *file_name);

void free_database(hash_t hash_table[]);

void file_t_print(file_t* head);

#endif
