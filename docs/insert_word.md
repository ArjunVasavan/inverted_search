# insert_word()

Core insertion function for a hash-based inverted index written in C.  
Tracks every word, which files contain it, and how many times it appears per file.

---

## Table of Contents

- [Overview](#overview)
- [Hash Function](#hash-function)
- [Data Structures](#data-structures)
- [The Three Cases](#the-three-cases)
- [Step-by-Step Walkthrough](#step-by-step-walkthrough)
- [Pseudocode](#pseudocode)
- [Variable Reference](#variable-reference)
- [Complexity](#complexity)
- [Common Pitfalls](#common-pitfalls)

---

## Overview

```c
status_t insert_word(hash_t *hash_table, const char *word, const char *file_name);
```

Given a `word` and the `file_name` it was found in, this function:

- Hashes the word to find the correct bucket
- Finds or creates a **main node** for the word
- Finds or creates a **sub node** for the file inside that word's list
- Increments the right counter — never duplicates entries

Returns `SUCCESS` in all valid cases.

---

## Hash Function

```c
int hash_function(const char *word);
```

Maps a word to a bucket by its **first character**:

```
'a' or 'A'  -->  bucket  0
'b' or 'B'  -->  bucket  1
...
'z' or 'Z'  -->  bucket 25
NULL/digit  -->  bucket 26  (catch-all)
```

The table has **27 buckets** total (`HASH_SIZE = 27`).  
All non-alphabetic words land in bucket 26.

---

## Data Structures

The database has three levels:

```
Level 1   hash_t   one bucket per letter (array of 27)
Level 2   main_t   one node per unique word
Level 3   sub_t    one node per file that contains the word
```

**`hash_t` — the bucket**

```
hash_t
  .index      bucket number (0..26)
  .head  -->  first main_t node in this bucket
```

**`main_t` — the word node**

```
main_t
  .word         the token (string)
  .file_count   number of distinct files containing this word
  .sub_link --> first sub_t node for this word
  .next     --> next word in the same bucket (collision chain)
```

**`sub_t` — the file node**

```
sub_t
  .file_name    file where the word appears
  .word_count   how many times the word appears in that file
  .next     --> next file node for the same word
```

**Invariants that must always hold:**

- `main_t.file_count` equals the number of `sub_t` nodes linked to it
- At most one `sub_t` per `(word, file_name)` pair
- Lists are always **appended**, never overwritten

---

## The Three Cases

### Case 1 — Bucket is empty

```
hash_table[index].head == NULL
```

The word has never been seen before. Allocate a new `main_t` and a new `sub_t`, link them, and attach to the bucket head.

```
Before:
  bucket[0].head  -->  NULL


After inserting "apple" from "file1.txt":
  bucket[0].head  -->  [ main_t ]  -->  NULL
                           |
                           v
                       [ sub_t ]  -->  NULL
```

---

### Case 2 — Word found in bucket

Walk the main list until `strcmp(cur_main->word, word) == 0`, then check the sub list.

**Sub-case A: same file seen again**

```
"apple" from "file1.txt"  (already exists)

sub_t found  -->  word_count++  -->  return SUCCESS
No new nodes. file_count unchanged.

  [ main_t: apple | fc=1 ]
        |
        v
  [ sub_t: file1.txt | wc=1 ]  -->  NULL
              |
             wc becomes 2
```

**Sub-case B: new file for an existing word**

```
"apple" from "file2.txt"  (word exists, file is new)

Sub list exhausted without finding file2.txt.
  -->  allocate new sub_t
  -->  prev_sub->next = new_sub_node
  -->  cur_main->file_count++

  [ main_t: apple | fc=1 --> fc=2 ]
        |
        v
  [ sub_t: file1.txt | wc=2 ]  -->  [ sub_t: file2.txt | wc=1 ]  -->  NULL
                                          ^
                                     newly appended
```

---

### Case 3 — Collision (word not found, bucket not empty)

Main list exhausted without finding the word.  
Allocate a new `main_t` and `sub_t`, append to the end of the main list via `prev_main->next`.

```
"ant" from "file1.txt"  (new word, but bucket 0 already has "apple")

  bucket[0].head
        |
        v
  [ main_t: apple ]  -->  [ main_t: ant ]  -->  NULL
        |                       |          ^
        v                       v     newly appended
  [ file1.txt | wc=2 ]    [ file1.txt | wc=1 ]  -->  NULL
        |
        v
  [ file2.txt | wc=1 ]  -->  NULL

Both words share bucket 0 but have completely independent sub lists.
```

---

## Step-by-Step Walkthrough

Four inserts into an empty table, showing exactly what changes each time.

```
insert 1:  insert_word(ht, "apple", "file1.txt")
insert 2:  insert_word(ht, "apple", "file1.txt")   same word, same file
insert 3:  insert_word(ht, "apple", "file2.txt")   same word, new file
insert 4:  insert_word(ht, "ant",   "file1.txt")   new word, same bucket
```

---

**After insert 1**

```
bucket[0].head
    |
    v
[ apple | fc=1 ]  -->  NULL
    |
    v
[ file1.txt | wc=1 ]  -->  NULL
```

---

**After insert 2** — only `word_count` changes, no new nodes

```
bucket[0].head
    |
    v
[ apple | fc=1 ]  -->  NULL
    |
    v
[ file1.txt | wc=2 ]  -->  NULL
              ^^^
            1  -->  2
```

---

**After insert 3** — new sub node appended, `file_count` incremented

```
bucket[0].head
    |
    v
[ apple | fc=2 ]  -->  NULL
    |        ^^^
    |       1 --> 2
    v
[ file1.txt | wc=2 ]  -->  [ file2.txt | wc=1 ]  -->  NULL
                                 ^
                           newly appended
```

---

**After insert 4** — new main node appended to bucket (collision)

```
bucket[0].head
    |
    v
[ apple | fc=2 ]  -->  [ ant | fc=1 ]  -->  NULL
    |                       |          ^
    v                       v     newly appended
[ file1.txt | wc=2 ]  [ file1.txt | wc=1 ]  -->  NULL
    |
    v
[ file2.txt | wc=1 ]  -->  NULL
```

---

## Pseudocode

```
insert_word(hash_table, word, file_name):

    index = hash_function(word)

    -- CASE 1: bucket is empty
    if hash_table[index].head == NULL:
        new_main_node = { word, file_count=1, sub_link=NULL, next=NULL }
        new_sub_node  = { file_name, word_count=1, next=NULL }
        new_main_node.sub_link    = new_sub_node
        hash_table[index].head    = new_main_node
        return SUCCESS

    -- CASE 2 + 3: traverse main list
    cur_main  = hash_table[index].head
    prev_main = cur_main

    while cur_main != NULL:

        if cur_main.word == word:          -- word found (Case 2)

            cur_sub  = cur_main.sub_link
            prev_sub = NULL

            while cur_sub != NULL:
                if cur_sub.file_name == file_name:
                    cur_sub.word_count++   -- same file, just count
                    return SUCCESS
                prev_sub = cur_sub
                cur_sub  = cur_sub.next

            -- file not in sub list: append new sub node
            new_sub_node = { file_name, word_count=1, next=NULL }
            prev_sub.next         = new_sub_node
            cur_main.file_count++
            return SUCCESS

        prev_main = cur_main
        cur_main  = cur_main.next

    -- word not found anywhere: append new main node (Case 3)
    new_main_node = { word, file_count=1, next=NULL }
    new_sub_node  = { file_name, word_count=1, next=NULL }
    new_main_node.sub_link = new_sub_node
    prev_main.next         = new_main_node
    return SUCCESS
```

---

## Variable Reference

| Variable        | Type      | Role                                          |
|-----------------|-----------|-----------------------------------------------|
| `hash_table`    | `hash_t*` | The hash table array passed to the function   |
| `index`         | `int`     | Bucket index from `hash_function()`           |
| `cur_main`      | `main_t*` | Current node while walking the main list      |
| `prev_main`     | `main_t*` | Previous node — used to append new main nodes |
| `cur_sub`       | `sub_t*`  | Current node while walking the sub list       |
| `prev_sub`      | `sub_t*`  | Previous node — used to append new sub nodes  |
| `new_main_node` | `main_t*` | Newly allocated word node                     |
| `new_sub_node`  | `sub_t*`  | Newly allocated file node                     |
| `word`          | `char*`   | Token being inserted                          |
| `file_name`     | `char*`   | File the token was found in                   |

---

## Complexity

| Phase              | Average  | Worst Case                         |
|--------------------|----------|------------------------------------|
| Hash lookup        | O(1)     | O(1)                               |
| Main list traverse | O(k)     | O(n) if all words share one bucket |
| Sub list traverse  | O(f)     | O(f) where f = files per word      |
| **Total**          | **O(1)** | **O(n)**                           |

Since this hash maps by first character only, performance depends on how evenly your input words are distributed across the alphabet.

---

## Common Pitfalls

**Overwriting `head` instead of appending**  
Always use `prev_main->next = new_node`. Never reassign `hash_table[index].head` after the initial setup.

**Forgetting `next = NULL` on new nodes**  
Every newly allocated node must have `next` set to `NULL` before being linked anywhere.

**Incrementing `file_count` before the allocation**  
Only increment `file_count` after `malloc` succeeds and the node is fully linked.

**Using `cur_sub = new_sub_node` to attach**  
At the end of the sub loop, `cur_sub` is `NULL`. Use `prev_sub->next = new_sub_node` to attach.

**Not initializing `prev_sub = NULL`**  
If the sub list has exactly one node, the loop runs once and exits. `prev_sub` must point to that node after — initialize it before the loop starts.

**Setting `sub_link` on an existing main node**  
Only set `sub_link` when creating a brand-new `main_t`. On an existing node, always append to the tail of its sub list.

---

*Part of the `inverted_search` project — a C implementation of an inverted index using a hash table with chained linked lists.*
