#include "array.h"
#include "stdlib.h"
#include "string.h"

void* _array_create(u64 len, u64 stride) {
    // allocate an array
    u64 total_size = (ARRAY_FIELDS_LENGTH * sizeof(u64)) + (len * stride);
    u64* ptr = malloc(total_size);
    ptr[ARRAY_CAPACITY] = len;
    ptr[ARRAY_LENGTH] = 0;
    ptr[ARRAY_STRIDE] = stride;

    // return the element array
    return (void*)(ptr + ARRAY_FIELDS_LENGTH);
}

void* _array_push(void* array, const void* value_ptr) {
    u64 len = *array_length(array);
    u64 stride = *array_stride(array);
    u64 cap = *array_capacity(array);

    if (len >= cap) {
        array = _array_resize(array);
    }

    u64 addr = (u64) array;
    addr += (len * stride);
    memcpy((void*) addr, value_ptr, stride);

    *array_length(array) = len + 1;

    return array;
}

void* _array_resize(void* array) {
    u64 len = *array_length(array);
    u64 stride = *array_stride(array);
    u64 new_cap = *array_capacity(array) * 2;

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
    u64* ptr = (u64*)array - ARRAY_FIELDS_LENGTH;
    free(ptr);
}


u64* array_length(void* array) {
    u64* ptr = (u64*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_LENGTH];
}
u64* array_capacity(void* array) {
    u64* ptr = (u64*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_CAPACITY];
}
u64* array_stride(void* array) {
    u64* ptr = (u64*)array - ARRAY_FIELDS_LENGTH;
    return &ptr[ARRAY_STRIDE];
}
