#include "array.h"
#include "stdlib.h"
#include "string.h"

void* _array_create(uint64_t len, uint64_t stride) {
    // allocate an array
    uint64_t total_size = (ARRAY_FIELDS_LENGTH * sizeof(uint64_t)) + (len * stride);
    uint64_t* ptr = malloc(total_size);
    ptr[ARRAY_CAPACITY] = len;
    ptr[ARRAY_LENGTH] = 0;
    ptr[ARRAY_STRIDE] = stride;

    // return the element array
    return (void*)(ptr + ARRAY_FIELDS_LENGTH);
}

void* _array_push(void* array, const void* value_ptr) {
    uint64_t len = *array_length(array);
    uint64_t stride = *array_stride(array);
    uint64_t cap = *array_capacity(array);

    if (len >= cap) {
        array = _array_resize(array);
    }

    uint64_t addr = (uint64_t) array;
    addr += (len * stride);
    memcpy((void*) addr, value_ptr, stride);

    *array_length(array) = len + 1;

    return array;
}

void* _array_resize(void* array) {
    uint64_t len = *array_length(array);
    uint64_t stride = *array_stride(array);
    uint64_t new_cap = *array_capacity(array) * 2;

    // create new array
    void* ptr = _array_create(new_cap, len * stride);
    // move it all over
    memcpy(ptr, array, len * stride);
    // free old one
    _array_destroy(array);

    return ptr;
}

void _array_destroy(void* array) {
    // given a pointer to the data we need to backtrack to the header
    uint64_t* ptr = (uint64_t*)array - ARRAY_FIELDS_LENGTH;
    free(ptr);
}


uint64_t* array_length(void* array) {
    uint64_t* ptr = (uint64_t*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_LENGTH];
}
uint64_t* array_capacity(void* array) {
    uint64_t* ptr = (uint64_t*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_CAPACITY];
}
uint64_t* array_stride(void* array) {
    uint64_t* ptr = (uint64_t*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_STRIDE];
}
