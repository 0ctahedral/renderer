#pragma once
#include "defines.h"

/*
 * This file creates types and interfaces for use across all backends
 */

// forward declarations
// TODO: get rid of this in favor of a window
// interface
typedef struct GLFWwindow GLFWwindow;

/// interface for all renderers to use
typedef struct renderer_backend_i {
    // TODO: remove this
    GLFWwindow* window;

    bool (*initialize)(GLFWwindow *window, const char* name);

    void (*shutdown)();

} renderer_backend_i;
