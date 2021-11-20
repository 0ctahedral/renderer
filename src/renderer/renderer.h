#pragma once

#include "../defines.h"

// only using this one because this should be the
// only place this is included
// subject to change
#include <vulkan/vulkan.h>

// forward declarations
typedef struct GLFWwindow GLFWwindow;

/// open a window
GLFWwindow* open_window();

// run main loop
int main_loop();

/// context
typedef struct vulkan_context {
    VkInstance instance;
    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_context;

/// initialize the renderer
bool initialize_renderer(const char *name);

/// shutdown the renderer
void shutdown_renderer(void);
