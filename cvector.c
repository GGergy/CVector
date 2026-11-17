//
// Created by Gergy on 11.11.2026.
//

#include <stdlib.h>
#include <string.h>

#include "cvector.h"


const size_t cv_npos = -1;

// ==== Internals ==== //

static const size_t MIN_CAPACITY = 8;

static size_t compute_next_cap(const size_t current_cap) {
    if (current_cap == 0) {
        return MIN_CAPACITY;
    }

    if (current_cap < 4096) {
        return current_cap * 2;
    }

    return current_cap + current_cap / 2;
}


static void cv_realloc(cvec_t *vec, const size_t new_size) {
    void *new_data = realloc(vec->data, new_size * vec->member);

    if (new_data != NULL) {
        vec->data = new_data;
        vec->_reserved = new_size;
    }
}

static void *at_insecure(const cvec_t *vec, const size_t index) {
    return (char *) vec->data + index * vec->member;
}


// ==== Default Comparators ==== //

int int_comparator(const void *a, const void *b) {
    const int ai = *((int *) a);
    const int bi = *((int *) b);

    if (ai < bi) return -1;
    if (ai > bi) return 1;
    return 0;
}

int double_comparator(const void *a, const void *b) {
    const double ad = *((double *) a);
    const double bd = *((double *) b);

    if (ad < bd) return -1;
    if (ad > bd) return 1;
    return 0;
}

int string_comparator(const void *a, const void *b) {
    const char *as = *((const char **) a);
    const char *bs = *((const char **) b);

    const int res = strcmp(as, bs);
    if (res < 0) return -1;
    if (res > 0) return 1;
    return 0;
}


// ==== Allocation ==== //

cvec_t *cv_new(const size_t member, int (*comparator)(const void *a, const void *b)) {
    cvec_t *vec = malloc(sizeof(cvec_t));

    vec->size = 0;
    vec->_reserved = 0;
    vec->member = member;
    vec->comparator = comparator;
    vec->data = NULL;

    return vec;
}

cvec_t *cv_from_arr(const void *arr, const size_t size, const size_t member,
                    int (*comparator)(const void *a, const void *b)) {
    cvec_t *vec = malloc(sizeof(cvec_t));

    vec->member = member;
    vec->size = size;
    vec->_reserved = size;
    vec->comparator = comparator;
    vec->data = malloc(size * member);

    memcpy(vec->data, arr, size * member);

    return vec;
}

void cv_destroy(cvec_t *vec) {
    free(vec->data);
    free(vec);
}


// ==== Element access ==== //

void *cv_at(const cvec_t *vec, const size_t index) {
    if (index >= vec->size) return NULL;

    return (char *) vec->data + index * vec->member;
}

void *cv_front(const cvec_t *vec) {
    if (!vec->size) return NULL;

    return vec->data;
}

void *cv_back(const cvec_t *vec) {
    if (!vec->size) return NULL;

    return (char *) vec->data + (vec->size - 1) * vec->member;
}


// ==== Capacity ==== //

bool cv_empty(const cvec_t *vec) {
    return vec->size == 0;
}

size_t cv_capacity(const cvec_t *vec) {
    return vec->_reserved;
}

size_t cv_size(const cvec_t *vec) {
    return vec->size;
}

void cv_reserve(cvec_t *vec, const size_t reserve) {
    if (vec->_reserved > reserve) return;

    cv_realloc(vec, reserve);
}


// ==== Modifiers ==== //

void cv_clear(cvec_t *vec) {
    vec->size = 0;
}

// Insecure
void cv_to(const cvec_t *vec, const void *data, const size_t index) {
    memcpy((char *) vec->data + index * vec->member, data, vec->member);
}

// Insecure
void cv_insert(cvec_t *vec, const void *data, const size_t index) {
    if (vec->_reserved == vec->size) {
        cv_realloc(vec, compute_next_cap(vec->size));
    }

    for (size_t i = vec->size - 1; i > index; i--) {
        cv_to(vec, at_insecure(vec, i), i + 1);
    }

    cv_to(vec, data, index);
    vec->size++;
}

// Insecure
void cv_erase(cvec_t *vec, const size_t index) {
    for (size_t i = index + 1; i < vec->size; i++) {
        cv_to(vec, at_insecure(vec, i), i - 1);
    }

    vec->size--;
}

void cv_push_back(cvec_t *vec, const void *data) {
    if (vec->_reserved == vec->size) {
        cv_realloc(vec, compute_next_cap(vec->size));
    }

    cv_to(vec, data, vec->size++);
}

// Insecure
void cv_pop_back(cvec_t *vec) {
    vec->size--;
}

void cv_resize(cvec_t *vec, const size_t new_size, const void *ini_value) {
    if (vec->size >= new_size) {
        vec->size = new_size;
        return;
    }
    cv_realloc(vec, new_size);
    for (size_t i = vec->size; i < new_size; i++) {
        cv_to(vec, ini_value, i);
    }
    vec->size = new_size;
}


void cv_swap(cvec_t *vec, cvec_t *other) {
    const cvec_t tmp = {
        .size = vec->size, .member = vec->member, ._reserved = vec->_reserved, .comparator = vec->comparator,
        .data = vec->data
    };

    vec->size = other->size;
    vec->member = other->member;
    vec->_reserved = other->_reserved;
    vec->comparator = other->comparator;
    vec->data = other->data;

    other->size = tmp.size;
    other->member = tmp.member;
    other->_reserved = tmp._reserved;
    other->comparator = tmp.comparator;
    other->data = tmp.data;
}


// ==== Algorithms ==== //

void cv_sort(const cvec_t *vec) {
    qsort(vec->data, vec->size, vec->member, vec->comparator);
}

size_t cv_find(const cvec_t *vec, const void *what) {
    for (size_t i = 0; i < vec->size; i++) {
        if (vec->comparator(at_insecure(vec, i), what) == 0) {
            return i;
        }
    }
    return cv_npos;
}
