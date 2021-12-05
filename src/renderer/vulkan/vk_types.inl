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

typedef struct vk_image {
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;
} vk_image;

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

    vk_swapchain_support_info swapchain_support;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceMemoryProperties memory;
    VkPhysicalDeviceFeatures features;
    VkFormat depth_format;

    i32 graphics_queue_idx;
    i32 present_queue_idx;
    i32 compute_queue_idx;
    i32 transfer_queue_idx;

    bool supports_device_local_host_visible;

    // logical
    VkDevice logical_device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkQueue compute_queue;
}vk_device;

// swapchain
typedef struct vk_swapchain {
    VkSurfaceFormatKHR img_format;
    u8 max_frames;
    VkSwapchainKHR handle;
    u32 img_count;
    VkImage* images;
    VkImageView* views;

    vk_image depth;
} vk_swapchain;

/// static context for the vulkan renderer
/// this holds the instance and other vulkan types we
/// need for opperation
typedef struct vulkan_context {
    // dims of framebuffer
    u32 fb_width;
    u32 fb_height;
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

    vk_swapchain swapchain;
    u32 image_idx;
    u32 current_frame;

    bool recreating_swapchain;

    i32 (*find_memory_index)(u32 type_filter, u32 property_flags);
} vulkan_context;
