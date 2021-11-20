#include <stdint.h>
#include "../defines.h"

// array type
//      u64      u64       u64 
// | capacity | length | stride | elements...|
// capacity: initial is some limit
// length: current length of the array
// stride: size of the element type
// 
// then we just cast the pointer to the type we want
//

enum {
    ARRAY_CAPACITY,
    ARRAY_LENGTH,
    ARRAY_STRIDE,
    ARRAY_FIELDS_LENGTH,
};

void* _array_create(u64 len, u64 stride);

#define array_create(type) \
    _array_create(32, sizeof(type))

#define array_reserve(capacity, type) \
    _array_create(capacity, sizeof(type))

void _array_destroy(void* array);
#define array_destroy(array) _array_destroy(array);

void* _array_push(void* array, const void* value_ptr);
#define array_push(array, value)      \
{                                     \
    typeof(value) tmp = value;        \
    array = _array_push(array, &tmp); \
}

void* _array_resize(void* array);

// helpers
u64* array_length(void* array);
u64* array_capacity(void* array);
u64* array_stride(void* array);
