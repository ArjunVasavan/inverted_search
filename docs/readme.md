# Inverted Search Engine

A command-line inverted index database written in C.  
Give it a set of `.txt` files and it builds a searchable index — telling you exactly which files contain a word and how many times it appears in each.

---

## Table of Contents

- [What It Does](#what-it-does)
- [How It Works](#how-it-works)
  - [Data Structure](#data-structure)
  - [Hash Function](#hash-function)
  - [Core Functions](#core-functions)
- [Project Structure](#project-structure)
- [Building and Running](#building-and-running)
- [Menu Options](#menu-options)
- [Save File Format](#save-file-format)
- [Known Limitations](#known-limitations)

---

## What It Does

```
$ ./a.out file1.txt file2.txt file3.txt

1. Create Database
2. Display Database
3. Search Database
4. Save Database
5. Update Database
6. Exit
Enter choice: 3
Enter word: hello

--------------------------------------------------
Word: hello
Total Files: 2

File Name            Occurrences
----------------------------------------
file1.txt            3
file3.txt            1
--------------------------------------------------
```

You pass `.txt` files as command-line arguments. The program reads every word from every file, builds an in-memory inverted index, and lets you search, display, save, and reload it.

---

## How It Works

### Data Structure

The database is a three-level structure:

```
hash_table[27]          one bucket per letter of the alphabet + catch-all
    |
    v
main_t  (word node)     one per unique word, chained inside its bucket
    |
    v
sub_t   (file node)     one per file that contains the word
```

A concrete example after indexing two files:

```
bucket['a' - 'a' = 0]
    |
    v
[ word="apple" | file_count=2 ]  -->  [ word="ant" | file_count=1 ]  -->  NULL
        |                                      |
        v                                      v
[ file1.txt | wc=3 ]                   [ file2.txt | wc=1 ]  -->  NULL
        |
        v
[ file2.txt | wc=1 ]  -->  NULL
```

Every `main_t` node keeps a count of how many distinct files contain that word (`file_count`).  
Every `sub_t` node keeps a count of how many times the word appears in that specific file (`word_count`).

---

### Hash Function

```c
int hash_function(const char *word);
```

Maps a word to a bucket index based on its **first character**:

```
'a' / 'A'  -->  0
'b' / 'B'  -->  1
...
'z' / 'Z'  -->  25
anything else  -->  26  (digits, punctuation, NULL)
```

Total buckets: **27** (`HASH_SIZE = 27`).  
Words starting with the same letter share a bucket and form a collision chain.

---

### Core Functions

#### `create_hash_table`

Initializes all 27 buckets. Sets every `head` to `NULL` and every `index` to its position. Must be called before any other database operation.

---

#### `validate_files`

```c
status_t validate_files(int argc, char **argv, file_t **head);
```

Walks the command-line arguments, filters for `.txt` files, tries to open each one, and builds a linked list of valid filenames (`file_t`). Files that cannot be opened are skipped with a warning. Returns `FAILURE` if no valid files are found.

```
argv:  [ "program", "a.txt", "b.txt", "notes.pdf", "c.txt" ]
                        |         |         |            |
                       OK        OK      skipped        OK

file_list:  a.txt  -->  b.txt  -->  c.txt  -->  NULL
```

---

#### `create_database`

```c
status_t create_database(hash_t *hash_table, file_t *file_head);
```

Iterates through every file in the file list. For each file, reads character by character using `getc`, assembles words by splitting on spaces and newlines, then calls `insert_word` for every token found.

```
file1.txt contents: "hello world hello"

Tokens produced:
  insert_word(ht, "hello", "file1.txt")
  insert_word(ht, "world", "file1.txt")
  insert_word(ht, "hello", "file1.txt")

Result:
  "hello"  -->  [ file1.txt | wc=2 ]
  "world"  -->  [ file1.txt | wc=1 ]
```

---

#### `insert_word`

```c
status_t insert_word(hash_t *hash_table, const char *word, const char *file_name);
```

The core engine. Three cases:

**Case 1 — Bucket is empty**

```
hash_table[index].head == NULL

Action:
  allocate main_t  { word, file_count=1 }
  allocate sub_t   { file_name, word_count=1 }
  link them, attach to bucket head
```

**Case 2 — Word exists in bucket**

Walk the main list until `word` is found, then check the sub list.

```
Sub-case A: file already in sub list
  --> cur_sub->word_count++
  --> return (no new nodes)

Sub-case B: file not in sub list
  --> allocate new sub_t, append via prev_sub->next
  --> cur_main->file_count++
```

**Case 3 — Collision (word not found)**

```
Main list exhausted, word not found.
  --> allocate main_t + sub_t
  --> append via prev_main->next
```

The `prev_main` / `prev_sub` pointer pattern is what makes appending safe — it always points to the last real node so the new node can be attached after the loop ends.

---

#### `search_database`

```c
status_t search_database(hash_t *hash_table, const char *word);
```

Hashes the word, walks the main list until it finds a match, then prints the entire sub list — one line per file with its occurrence count. Returns `DATA_NOT_FOUND` if the word is not in the index.

---

#### `display_database`

Iterates all 27 buckets. Skips empty ones. For each non-empty bucket, prints every word and its file list. Useful for seeing the full state of the index.

---

#### `save_database`

```c
status_t save_database(hash_t *hash_table, const char *file_name);
```

Serializes the entire database to a text file. Each word gets one line in this format:

```
#<bucket_index>;<word>;<file_count>;<file1>;<wc1>;<file2>;<wc2>;...#
```

Example:

```
#0;apple;2;file1.txt;3;file2.txt;1;#
#0;ant;1;file2.txt;4;#
#7;hello;1;file1.txt;2;#
```

---

#### `update_database`

```c
status_t update_database(hash_t *hash_table, const char *file_name, file_t **file_list_head);
```

Reads a file previously written by `save_database` and reconstructs the database from it. For each line it parses the bucket index, word, file count, and all file/count pairs, then rebuilds the `main_t` and `sub_t` nodes directly. Also removes loaded filenames from the input file list to avoid double-counting.

---

#### `free_database`

```c
void free_database(hash_t *hash_table);
```

Walks every bucket, frees every `sub_t` node first, then every `main_t` node. Sets each bucket `head` back to `NULL`. Order matters — freeing `main_t` first would lose the pointer to its sub list.

```
For each bucket:
  cur_main  -->  cur_sub  -->  free sub nodes first
             -->  free main node after all its subs are gone
```

---

## Project Structure

```
inverted_search/
  include/
    inverted_search.h     all structs, enums, and function declarations
  src/
    main.c                menu loop and state machine
    hash.c                create_hash_table, hash_function
    insert_word.c         insert_word
    create_database.c     create_database
    search_database.c     search_database
    display_database.c    display_database
    save_database.c       save_database
    update_database.c     update_database, remove_file_from_list
    free_database.c       free_database
    validate_files.c      validate_files
    file_t_print.c        file_t_print (debug helper)
  build/                  object files (generated)
  Makefile
```

---

## Building and Running

**Requirements:** GCC, Make

```bash
# Build
make

# Run with text files
./a.out file1.txt file2.txt file3.txt

# Clean build artifacts
make clean
```

The Makefile compiles all `.c` files in `src/` into object files under `build/` and links them into `a.out`.

---

## Menu Options

| Option | Name | Allowed When | What It Does |
|--------|------|--------------|--------------|
| 1 | Create Database | `DB_EMPTY` or `DB_UPDATED` | Reads all input files and builds the index |
| 2 | Display Database | Any | Prints every word and its file list |
| 3 | Search Database | Any | Looks up a single word and prints results |
| 4 | Save Database | Any | Serializes the index to a file you name |
| 5 | Update Database | `DB_EMPTY` | Loads a previously saved database file |
| 6 | Exit | Any | Frees all memory and exits |

The program tracks database state using `db_state_t`:

```c
typedef enum {
    DB_EMPTY,
    DB_CREATED,
    DB_UPDATED
} db_state_t;
```

This prevents operations like creating the database twice without clearing it first.

---

## Save File Format

The save file is human-readable plain text. Each line represents one word entry:

```
#<index>;<word>;<file_count>;<file_name>;<word_count>;...#
```

Real example with two words:

```
#0;apple;2;file1.txt;3;file2.txt;1;#
#7;hello;1;notes.txt;5;#
```

The leading `#` and trailing `#` act as line sentinels so `update_database` can detect and skip malformed lines.

---

## Known Limitations

**`strstr` bug in `validate_files`**  
The check `strstr(argv[i], ".txt") == 0` compares a pointer to zero instead of `NULL`. On most systems a valid pointer is never zero so `.txt` files are accepted, but filenames like `notes.txt123` are also accepted. The fix is `== NULL`.

**No `malloc` failure handling in `insert_word`**  
If `malloc` returns `NULL` the function dereferences it immediately, causing a segfault. Each allocation should be checked before use.

**No word length guard in `create_database`**  
If a token in a file exceeds `WORD_SIZE - 1` (99) characters the `word` buffer overflows. A bounds check on `i` before `word[i++] = ch` would prevent this.

**`strtok` in `update_database` is not reentrant**  
If the save file is malformed, `strtok` can return `NULL` and the subsequent `strcpy` or `atoi` call will crash. Input validation on each token would make loading robust.

---

*Built as a learning project in C — covers hash tables, linked lists, file I/O, dynamic memory, and serialization.*
