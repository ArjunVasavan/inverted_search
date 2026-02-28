# Inverted Search Engine (C)

![C](https://img.shields.io/badge/language-C-blue?style=flat-square)
![Status](https://img.shields.io/badge/status-completed-brightgreen?style=flat-square)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)

---

A file-based **Inverted Index implementation in C** using a hash table and two-level linked lists.

The system indexes words from multiple text files and stores:

* Number of files containing each word
* Word frequency per file

Supports database creation, search, save, update, and memory cleanup.

---

## Structure

```
hash_table[index]
    ↓
 main_node (word, file_count)
    ↓
 sub_node (file_name, word_count)
```

* Hash table size: 27
* Collision handling: chaining
* Multi-level linked list design

---

## Project Layout

```
.
├── include/
│   └── inverted_search.h
├── src/
│   ├── main.c
│   ├── file_validation.c
│   ├── hash_table.c
│   ├── insert_word.c
│   ├── create_db.c
│   ├── display.c
│   ├── search.c
│   ├── save_db.c
│   ├── update_db.c
│   └── free_db.c
├── data/
│   ├── input/      # Place input text files here
│   └── saved_db/   # Saved database files stored here
├── Makefile
└── readme.md
```

---

## Build

```bash
make
```

---

## Run

```bash
./a.out data/input/file1.txt data/input/file2.txt
```

---

## Save Format

Database is stored as:

```
#index;word;file_count;file1;count1;file2;count2;...;#
```

Example:

```
#0;apple;3;file1.txt;3;file2.txt;2;file3.txt;1;#
```

---

## Author

Arjun Vasavan
