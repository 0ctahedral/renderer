#pragma once

#include "defines.h"
#include "renderer/renderer_types.inl"

// only using this one because this should be the
// only place this is included
// subject to change
#include <vulkan/vulkan.h>

// forward declarations
typedef struct GLFWwindow GLFWwindow;

// run main loop
int main_loop();

/// context
typedef struct vulkan_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_context;

/// initialize the renderer
bool vk_initialize(renderer_backend* backend, const char* name);

/// shutdown the renderer
void vk_shutdown(renderer_backend* backend);
