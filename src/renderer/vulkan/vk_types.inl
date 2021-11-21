#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
/*
 * Types for use in the vulkan backend
 */



/// static context for the vulkan renderer
/// this holds the instance and other vulkan types we
/// need for opperation
typedef struct vulkan_context {
    /// handle to our vulkan instance
    VkInstance instance;

    /// Surface we are presenting on
    VkSurfaceKHR surface;

    /// allocator, TODO: make an allocator
    VkAllocationCallbacks* allocator;

    /// reference to the debug debug messenger
    /// that will log for us
    VkDebugUtilsMessengerEXT debug_messenger;

} vulkan_context;
