#pragma once

#include "vk_types.inl"

bool vk_device_create(vulkan_context* context);

void vk_device_destroy(vulkan_context* context);

void vk_query_swapchain_support(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    vk_swapchain_support_info* out_support_info
);

bool vk_detect_depth_format(vk_device* device);
