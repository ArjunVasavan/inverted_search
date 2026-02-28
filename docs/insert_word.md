# `insert_word()` — Inverted Search Database

> Core insertion engine for a hash-based inverted index.  
> Tracks every word, which files contain it, and how many times it appears in each.

---

## Table of Contents

- [Overview](#overview)
- [Hash Function](#hash-function)
- [Data Structures](#data-structures)
- [How It Works](#how-it-works)
  - [Case 1 — Empty Bucket](#case-1--empty-bucket)
  - [Case 2 — Word Already Exists](#case-2--word-already-exists)
  - [Case 3 — Collision (New Word, Same Bucket)](#case-3--collision-new-word-same-bucket)
- [Visual Walkthrough](#visual-walkthrough)
- [Full Pseudocode](#full-pseudocode)
- [Complexity](#complexity)
- [Common Pitfalls](#common-pitfalls)

---

## Overview

```c
status_t insert_word(hash_t *hash_table, const char *word, const char *file_name);
```

Given a `word` and the `file_name` it came from, this function:

1. Hashes the word to find the right bucket.
2. Finds (or creates) a **main node** for the word.
3. Finds (or creates) a **sub node** for the file inside that word's list.
4. Increments the appropriate counter — never duplicates entries.

Returns `SUCCESS` in all valid cases.

---

## Hash Function

```c
int hash_function(const char *word);
```

Maps a word to a bucket index `[0..26]`:

| First Character  | Bucket Index     |
|------------------|------------------|
| `'a'` or `'A'`   | `0`              |
| `'b'` or `'B'`   | `1`              |
| …                | …                |
| `'z'` or `'Z'`   | `25`             |
| `NULL` or digit  | `26` (catch-all) |

The table has **27 buckets** (`HASH_SIZE = 27`). All non-alpha words land in bucket 26.

---

## Data Structures

```
 hash_t          main_t                    sub_t
┌──────────┐    ┌───────────────────┐    ┌──────────────────┐
│ index    │    │ word[100]         │    │ file_name[100]   │
│ head  ───┼───▶│ file_count        │    │ word_count       │
└──────────┘    │ sub_link  ────────┼───▶│ next ────────────┼──▶ ...
                │ next ─────────────┼──▶ └──────────────────┘
                └───────────────────┘   (one node per file)
               (one node per word)
```

| Struct   | Field        | Meaning                                       |
|----------|--------------|-----------------------------------------------|
| `hash_t` | `index`      | Bucket number `0..26`                         |
| `hash_t` | `head`       | Pointer to first word node in this bucket     |
| `main_t` | `word`       | The token stored in this node                 |
| `main_t` | `file_count` | How many distinct files contain this word     |
| `main_t` | `sub_link`   | Head of the file list for this word           |
| `main_t` | `next`       | Next word node in the same bucket (collision) |
| `sub_t`  | `file_name`  | Name of the file this node represents         |
| `sub_t`  | `word_count` | How many times the word appears in that file  |
| `sub_t`  | `next`       | Next file node for the same word              |

**Invariants that must always hold:**
- `main_t.file_count` == number of `sub_t` nodes linked to it
- Exactly one `sub_t` per `(word, file_name)` pair
- Sub-list is **appended**, never overwritten

---

## How It Works

### Case 1 — Empty Bucket

```
hash_table[index].head == NULL
```

The word has never been seen. Create everything from scratch.

```
hash_table[index].head
        │
        ▼
  ┌─────────────────────┐
  │ main_t              │──next──▶ NULL
  │ word      = "apple" │
  │ file_count = 1      │
  │ sub_link ───────────┼──▶ ┌────────────────────────┐
  └─────────────────────┘    │ sub_t                  │──next──▶ NULL
                             │ file_name = "file1.txt"│
                             │ word_count = 1         │
                             └────────────────────────┘
```

---

### Case 2 — Word Already Exists

Walk the main list until `strcmp(cur_main->word, word) == 0`.

#### Sub-case A — Same file seen again

`strcmp(cur_sub->file_name, file_name) == 0`  
→ Just increment `word_count`. Nothing else changes.

```
Before:  [file1.txt | wc=1] ──▶ NULL
After:   [file1.txt | wc=2] ──▶ NULL   ← only word_count changed
```

#### Sub-case B — New file for existing word

The sub-list is exhausted without finding the file.  
→ Create a new `sub_t`, attach it to the end via `prev_sub->next`, then increment `file_count`.

```
Before:  file_count=1
         [file1.txt | wc=2] ──▶ NULL

After:   file_count=2
         [file1.txt | wc=2] ──▶ [file2.txt | wc=1] ──▶ NULL
                                  ↑ newly appended
```

---

### Case 3 — Collision (New Word, Same Bucket)

The main list is exhausted without finding the word.  
→ Create a new `main_t` + `sub_t` pair, and attach to the end via `prev_main->next`.

```
hash_table[0].head
        │
        ▼
  [main: "apple"] ──next──▶ [main: "ant"] ──next──▶ NULL
         │                         │
         ▼                         ▼
  [file1.txt | wc=2]         [file1.txt | wc=1]
         │                         │
         ▼                         ▼
  [file2.txt | wc=1]             NULL
         │
         ▼
        NULL
```

Both words share bucket `0` (`'a'`), but each has its own independent sub-list.

---

## Visual Walkthrough

Four sequential inserts, step by step:

```
① insert_word(ht, "apple", "file1.txt")
② insert_word(ht, "apple", "file1.txt")   ← same word, same file
③ insert_word(ht, "apple", "file2.txt")   ← same word, new file
④ insert_word(ht, "ant",   "file1.txt")   ← new word, same bucket
```

---

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
After ①
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  bucket[0].head
      │
      ▼
  [apple | fc=1] ──▶ NULL
      │
      ▼
  [file1.txt | wc=1] ──▶ NULL

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
After ②  (word_count incremented, nothing else changes)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  [apple | fc=1] ──▶ NULL
      │
      ▼
  [file1.txt | wc=2] ──▶ NULL
                ↑
          wc: 1 → 2

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
After ③  (new sub node appended, file_count incremented)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  [apple | fc=2] ──▶ NULL
      │         ↑
      │     fc: 1 → 2
      ▼
  [file1.txt | wc=2] ──▶ [file2.txt | wc=1] ──▶ NULL
                                ↑
                          newly appended

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
After ④  (collision: new main node appended to bucket)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  bucket[0].head
      │
      ▼
  [apple | fc=2] ──next──▶ [ant | fc=1] ──▶ NULL
      │                         │           ↑
      ▼                         ▼      newly appended
  [file1.txt|wc=2]──▶[file2.txt|wc=1]──▶NULL
                      [file1.txt|wc=1]──▶NULL
```

---

## Full Pseudocode

```
insert_word(hash_table, word, file_name):

  index ← hash_function(word)

  ── CASE 1: bucket is empty ──────────────────────────────────
  if hash_table[index].head == NULL:
      new_main_node ← { word, file_count=1, next=NULL }
      new_sub_node  ← { file_name, word_count=1, next=NULL }
      new_main_node.sub_link ← new_sub_node
      hash_table[index].head ← new_main_node
      return SUCCESS

  ── CASE 2 & 3: traverse main list ───────────────────────────
  cur_main  ← hash_table[index].head
  prev_main ← cur_main

  while cur_main ≠ NULL:

      if cur_main.word == word:            ── CASE 2: word found

          cur_sub  ← cur_main.sub_link
          prev_sub ← NULL

          while cur_sub ≠ NULL:
              if cur_sub.file_name == file_name:
                  cur_sub.word_count++     ── same file: just count
                  return SUCCESS
              prev_sub ← cur_sub
              cur_sub  ← cur_sub.next

          ── file not found: append new sub node
          new_sub_node ← { file_name, word_count=1, next=NULL }
          prev_sub.next         ← new_sub_node
          cur_main.file_count++
          return SUCCESS

      prev_main ← cur_main
      cur_main  ← cur_main.next

  ── CASE 3: word not found → append new main node ────────────
  new_main_node ← { word, file_count=1, next=NULL }
  new_sub_node  ← { file_name, word_count=1, next=NULL }
  new_main_node.sub_link ← new_sub_node
  prev_main.next         ← new_main_node
  return SUCCESS
```

---

## Complexity

| Phase              | Average  | Worst Case                           |
|--------------------|----------|--------------------------------------|
| Hash lookup        | O(1)     | O(1)                                 |
| Main list traverse | O(k)     | O(n) — all words hash to same bucket |
| Sub list traverse  | O(f)     | O(f) — f files for one word          |
| **Total**          | **O(1)** | **O(n)**                             |

With a good hash function and reasonable input, `k` and `f` stay very small.  
This hash maps by **first character only**, so bucket load depends entirely on word distribution in your input files.

---

## Common Pitfalls

| Mistake | Fix |
|--------|-----|
| Reassigning `head` instead of appending | Use `prev_main->next = new_node` — never touch `head` after init |
| Forgetting `new_node->next = NULL` | Always initialize `next` on every newly allocated node |
| Incrementing `file_count` before `malloc` | Increment only **after** allocation and linking succeed |
| Using `cur_sub = new_sub_node` to attach | Use `prev_sub->next = new_sub_node` — `cur_sub` is `NULL` at loop end |
| Not initializing `prev_sub = NULL` | Required — if sub-list has exactly one node, `prev_sub` must be valid |
| Overwriting `sub_link` on an existing node | Only set `sub_link` when creating a **brand new** main node |

---

*Part of the inverted_search project — a C implementation of an inverted index using a hash table with chained linked lists.*
