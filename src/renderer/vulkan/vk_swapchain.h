#pragma once
#include "vk_types.inl"
#include "defines.h"

void vk_swapchain_create(
    vulkan_context* context,
    u32 width,
    u32 height,
    vk_swapchain* out_swapchain
);


void vk_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vk_swapchain* swapchain
);

void vk_swapchain_destroy(
    vulkan_context* context,
    vk_swapchain* swapchain
);

bool vk_swapchain_next_image_index(
    vulkan_context* context,
    vk_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore img_avail,
    VkFence fence,
    u32* out_idx
);

void vk_swapchain_present(
    vulkan_context* context,
    vk_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete,
    u32 present_img_idx
);
