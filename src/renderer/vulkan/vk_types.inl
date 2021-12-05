#pragma once

#include "defines.h"
#include "asserts.h"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
/*
 * Types for use in the vulkan backend
 */

#define VK_CHECK(expr)          \
{                               \
    assert(expr == VK_SUCCESS); \
}

typedef struct vk_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vk_swapchain_support_info;

/// Encapsulates the physical and logical device
typedef struct vk_device {
    // physdev and proerties
    VkPhysicalDevice physical_device;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory;
    VkPhysicalDeviceFeatures features;

    vk_swapchain_support_info swapchain_support;

    i32 graphics_queue_idx;
    i32 present_queue_idx;
    i32 compute_queue_idx;
    i32 transfer_queue_idx;

    bool supports_device_local_host_visible;

    // logical

    VkDevice logical_device;
}vk_device;


/// static context for the vulkan renderer
/// this holds the instance and other vulkan types we
/// need for opperation
typedef struct vulkan_context {
    /// handle to our vulkan instance
    VkInstance instance;

    /// Surface we are presenting on
    VkSurfaceKHR surface;

    vk_device device;

    /// allocator, TODO: make an allocator
    VkAllocationCallbacks* allocator;

    /// reference to the debug debug messenger
    /// that will log for us
    VkDebugUtilsMessengerEXT debug_messenger;

} vulkan_context;
