#pragma once

#include <vulkan/vulkan.h>
// TODO: move glfw outta here
#include <GLFW/glfw3.h>
#include "defines.h"
#include "renderer/renderer_types.inl"

// forward declarations
typedef struct GLFWwindow GLFWwindow;

// run main loop
int main_loop();

/// context
typedef struct vulkan_context {
    VkInstance instance;

    VkSurfaceKHR surface;

    VkAllocationCallbacks* allocator;
    VkDebugUtilsMessengerEXT debug_messenger;
} vulkan_context;

/// initialize the renderer
bool vk_initialize(renderer_backend* backend, const char* name);

/// shutdown the renderer
void vk_shutdown(renderer_backend* backend);
