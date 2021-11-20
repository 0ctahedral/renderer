#pragma once

#include <assert.h>

#define assertf(A, M, ...) if(!(A)) {fprintf(stderr, "%s:%d: " M "\n", __FILE__, __LINE__,##__VA_ARGS__); assert(A); }
