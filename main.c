#include <stdio.h>
#include <stdlib.h>

#include "cvector.h"

#define iWrap(x) *(int *)(x)

int main(void) {
    // Vector from array
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const cvec_t *vec = cv_from_arr(arr, 10, sizeof(int), int_comparator);
    printf("Vector from array: [");
    for (size_t i = 0; i < vec->size; i++) {
        printf("%d ", iWrap(cv_at(vec, i)));
    }
    printf("]\nFront - %d; Back - %d\n\n", iWrap(cv_front(vec)), iWrap(cv_back(vec)));

    // Vector from stdin
    printf("Enter nums and 0 to end... ");
    int var;
    cvec_t *vec2 = cv_new(sizeof(int), int_comparator);
    while (scanf_s("%d", &var), var != 0) {
        cv_push_back(vec2, &var);
    }
    printf("What do you want to find?");
    int target;
    scanf_s("%d", &target);
    size_t pos = cv_find(vec2, &target);
    if (pos != cv_npos) {
        printf("Found %d at position %lld\n", target, pos);
    } else {
        printf("Not found\n");
    }

    return 0;
}
