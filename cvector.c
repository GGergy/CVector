//
// Created by Gergy on 11.11.2026.
//

#include <stdlib.h>
#include <string.h>

#include "cvector.h"


const size_t cv_npos = -1;


// ==== Internals ==== //

static const size_t MIN_CAPACITY = 4;

static size_t compute_next_cap(const size_t current_cap) {
    if (current_cap < MIN_CAPACITY) {
        return MIN_CAPACITY;
    }

    if (current_cap < 4096) {
        return current_cap * 2;
    }

    return current_cap + current_cap / 2;
}

static void cv_realloc_(cvec_t *vec, const size_t new_size) {
    void *new_data = realloc(vec->data, new_size * vec->member);

    if (new_data != NULL) {
        vec->data = new_data;
        vec->_reserved = new_size;
    }
}


// ==== Default Comparators ==== //

int int_comparator(const void *a, const void *b) {
    const int ai = iWrap(a);
    const int bi = iWrap(b);

    if (ai < bi) return -1;
    if (ai > bi) return 1;
    return 0;
}

int double_comparator(const void *a, const void *b) {
    const double ad = dWrap(a);
    const double bd = dWrap(b);

    if (ad < bd) return -1;
    if (ad > bd) return 1;
    return 0;
}

int string_comparator(const void *a, const void *b) {
    const char *as = sWrap(a);
    const char *bs = sWrap(b);

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


// ==== Iterators ==== //
void *cv_begin(const cvec_t *vec) {
    return vec->data;
}


void *cv_end(const cvec_t *vec) {
    return (char *) vec->data + vec->member * vec->size;
}


void *cv_rbegin(const cvec_t *vec) {
    return (char *) vec->data + vec->member * (vec->size - 1);
}

void *cv_rend(const cvec_t *vec) {
    return (char *) vec->data - vec->member;
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

    cv_realloc_(vec, reserve);
}

void cv_collapse(cvec_t *vec) {
    cv_realloc_(vec, vec->size);
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
        cv_realloc_(vec, compute_next_cap(vec->size));
    }

    const void *end = (char *) vec->data + index * vec->member;
    char *it;
    for (it = cv_end(vec); it != end; it -= vec->member) {
        memcpy(it, it - vec->member, vec->member);
    }

    memcpy(it, data, vec->member);
    vec->size++;
}

void cv_insert_it(cvec_t *vec, const void *data, void *iter) {
    if (vec->_reserved == vec->size) {
        cv_realloc_(vec, compute_next_cap(vec->size));
    }

    for (char *it = cv_end(vec); it != iter; it -= vec->member) {
        memcpy(it, it - vec->member, vec->member);
    }

    memcpy(iter, data, vec->member);
    vec->size++;
}


void cv_extend(cvec_t *vec, const cvec_t *other, const size_t index) {
    cv_reserve(vec, vec->size + other->size);

    const void *end = (char *) vec->data + (index - 1) * vec->member;
    char *it;
    const size_t axis = other->size * vec->member;
    for (it = cv_rbegin(vec); it != end; it -= vec->member) {
        memcpy(it + axis, it, vec->member);
    }
    for (char *oit = cv_begin(other); oit != cv_end(other); oit += vec->member) {
        it += vec->member;
        memcpy(it, oit, vec->member);
    }
}


// Extending vector with other vector before iterator
void cv_extend_it(cvec_t *vec, const cvec_t *other, void *iter) {
    cv_reserve(vec, vec->size + other->size);

    const char *end = (char *) iter - 1;
    const size_t axis = other->size * vec->member;
    for (char *it = cv_rbegin(vec); it != end; it -= vec->member) {
        memcpy(it + axis, it, vec->member);
    }

    char *it = iter;
    for (char *oit = cv_begin(other); oit != cv_end(other); oit += vec->member, it += vec->member) {
        memcpy(it, oit, vec->member);
    }
    vec->size += other->size;
}


// Extending vector with other vector in the end
void cv_extend_back(cvec_t *vec, const cvec_t *other) {
    cv_reserve(vec, vec->size + other->size);

    char *it = cv_end(vec);
    for (char *oit = cv_begin(other); oit != cv_end(other); oit += vec->member, it += vec->member) {
        memcpy(it, oit, vec->member);
    }

    vec->size += other->size;
}


// Insecure
void cv_erase(cvec_t *vec, const size_t index) {
    const void *end = cv_end(vec);
    for (char *it = (char *) vec->data + vec->member * index + 1; it != end; it += vec->member) {
        memcpy(it - vec->member, it, vec->member);
    }

    vec->size--;
}

void cv_push_back(cvec_t *vec, const void *data) {
    if (vec->_reserved == vec->size) {
        cv_realloc_(vec, compute_next_cap(vec->size));
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
    cv_realloc_(vec, new_size);

    const void *end = (char *) vec->data + new_size * vec->member;
    for (char *it = cv_rbegin(vec); it != end; it += vec->member) {
        memcpy(it, ini_value, vec->member);
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

void cv_reverse(const cvec_t *vec) {
    void *buffer = malloc(vec->member);
    char *front = cv_begin(vec);
    char *back = cv_rbegin(vec);

    for (size_t i = 0; i != vec->size / 2; i++) {
        const size_t axis = vec->member * i;

        memcpy(buffer, back - axis, vec->member);
        memcpy(back - axis, front + axis, vec->member);
        memcpy(front + axis, buffer, vec->member);
    }
}


void cv_erase_it(cvec_t *vec, void *iter) {
    const void *end = cv_end(vec);
    for (char *it = (char *) iter + vec->member; it != end; it += vec->member) {
        memcpy(it - vec->member, it, vec->member);
    }

    vec->size--;
}

// Remove by value, first left
bool cv_remove(cvec_t *vec, void *what) {
    const void *end = cv_end(vec);

    for (char *it = cv_begin(vec); it != end; it += vec->member) {
        if (vec->comparator(it, what) == 0) {
            for (char *jt = it + vec->member; jt != end; jt += vec->member) {
                memcpy(jt - vec->member, jt, vec->member);
            }
            return true;
        }
    }

    return false;
}


// ==== Algorithms ==== //

void cv_sort(const cvec_t *vec) {
    qsort(vec->data, vec->size, vec->member, vec->comparator);
}

size_t cv_find(const cvec_t *vec, const void *what) {
    const void *end = cv_end(vec);
    size_t index = 0;

    for (const char *it = cv_begin(vec); it != end; it += vec->member) {
        if (vec->comparator(it, what) == 0) {
            return index;
        }
        ++index;
    }
    return cv_npos;
}

size_t cv_rfind(const cvec_t *vec, const void *what) {
    if (vec->size == 0) return cv_npos;

    const void *end = cv_rend(vec);
    size_t index = vec->size - 1;

    for (const char *it = cv_rbegin(vec); it != end; it -= vec->member) {
        if (vec->comparator(it, what) == 0) {
            return index;
        }
        index--;
    }
    return cv_npos;
}

size_t cv_count(const cvec_t *vec, const void *what) {
    size_t count = 0;

    const void *end = cv_end(vec);

    for (const char *it = cv_begin(vec); it != end; it += vec->member) {
        if (vec->comparator(it, what) == 0) {
            count++;
        }
    }
    return count;
}

cvec_t *cv_copy(const cvec_t *vec) {
    cvec_t *new_vec = cv_new(vec->member, vec->comparator);
    cv_reserve(new_vec, vec->size);

    char *oit = new_vec->data;
    for (char *it = cv_begin(vec); it != cv_end(vec); it += vec->member, oit += vec->member) {
        memcpy(oit, it, vec->member);
    }
    new_vec->size = vec->size;

    return new_vec;
}

void cv_apply(const cvec_t *vec, void (*f)(void *a)) {
    for (char *it = cv_begin(vec); it != cv_end(vec); it += vec->member) {
        f(it);
    }
}
