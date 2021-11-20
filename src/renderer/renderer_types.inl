#pragma once
#include "defines.h"

/*
 * This file creates types and interfaces for use across all backends
 */

/// 
typedef struct renderer_backend {
    // data in common
    u64 current_frame;

    bool (*initialize)(struct renderer_backend* backend, const char* name);

    void (*shutdown)(struct renderer_backend* backend);

} renderer_backend;
