//
// Created by Gergy on 11.11.2026.
//
#pragma once

#ifndef CVECTOR_CVECTOR_H
#define CVECTOR_CVECTOR_H

#include <stdbool.h>


#define iWrap(x) *(int *)(x)
#define dWrap(x) *(double *)(x)
#define sWrap(x) *(char **)(x)

typedef struct cvector {
    size_t size; // Current num of elements
    size_t member; // Size of each element
    size_t _reserved; // Reserved memory (in units of elements)

    int (*comparator)(const void *a, const void *b);

    void *data;
} cvec_t;

extern const size_t cv_npos; // Like std::string::npos, position of not found element

// ==== Allocation ==== //

// Allocate new CVector
cvec_t *cv_new(size_t member, int (*comparator)(const void *a, const void *b));

// Allocate and initialize new CVector
cvec_t *cv_from_arr(const void *arr, size_t size, size_t member, int (*comparator)(const void *a, const void *b));

// Deleting existing cvector
void cv_destroy(cvec_t *vec);


// ==== Iterators ==== //

// Pointer on first element
void *cv_begin(const cvec_t *vec);

// Pointer after last element
void *cv_end(const cvec_t *vec);

// Pointer on last element
void *cv_rbegin(const cvec_t *vec);

// Pointer before first element
void *cv_rend(const cvec_t *vec);


// ==== Element access ==== //

// Access specified element with bounds checking
void *cv_at(const cvec_t *vec, size_t index);

// Access the first element
void *cv_front(const cvec_t *vec);

// Access the last element
void *cv_back(const cvec_t *vec);


// ==== Capacity ==== //

// Checks whether the container is empty
bool cv_empty(const cvec_t *vec);

// Reserves storage
void cv_reserve(cvec_t *vec, size_t reserve);

// Returns the number of elements that can be held in currently allocated storage
size_t cv_capacity(const cvec_t *vec);

// Returns the number of elements
size_t cv_size(const cvec_t *vec);

// Reduces capacity to size
void cv_collapse(cvec_t *vec);


// ==== Modifiers ==== //

// Clears the contents
void cv_clear(cvec_t *vec);

// Assign new value to element on index
void cv_to(const cvec_t *vec, const void *data, size_t index);

// Inserts elements at index
void cv_insert(cvec_t *vec, const void *data, size_t index);

// Inserts elements before iterator
void cv_insert_it(cvec_t *vec, const void *data, void *iter);

// Extending vector with other vector at index
void cv_extend(cvec_t *vec, const cvec_t *other, size_t index);

// Extending vector with other vector before iterator
void cv_extend_it(cvec_t *vec, const cvec_t *other, void *iter);

// Extending vector with other vector in the end
void cv_extend_back(cvec_t *vec, const cvec_t *other);

// Erases element at index
void cv_erase(cvec_t *vec, size_t index);

// Erases element at iterator
void cv_erase_it(cvec_t *vec, void *iter);

// Remove by value, first left
bool cv_remove(cvec_t *vec, void *what);

// Adds an element to the end
void cv_push_back(cvec_t *vec, const void *data);

// Removes the last element
void cv_pop_back(cvec_t *vec);

// Changes the number of elements stored
void cv_resize(cvec_t *vec, size_t new_size, const void *ini_value);

// Swaps the contents between vectors
void cv_swap(cvec_t *vec, cvec_t *other);


// ==== Algorithms ==== //

// Sorts vector
void cv_sort(const cvec_t *vec);

// Returns index of first entry of target
size_t cv_find(const cvec_t *vec, const void *what);

// Returns index of last entry of target
size_t cv_rfind(const cvec_t *vec, const void *what);

// Returns count of target
size_t cv_count(const cvec_t *vec, const void *what);

// Full copy
cvec_t *cv_copy(const cvec_t *vec);

// Reverse data
void cv_reverse(const cvec_t *vec);

// Applying function to every value in vector
void cv_apply(const cvec_t *vec, void (*f)(void *a));


// ==== Default Comparators ==== //
// Comparator must take params with exact same size as stored in vector

int int_comparator(const void *a, const void *b);

int double_comparator(const void *a, const void *b);

int string_comparator(const void *a, const void *b);


#endif //CVECTOR_CVECTOR_H
