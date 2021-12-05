#pragma once

#include "vk_types.inl"

void vk_image_create(
    vulkan_context* context,
    VkImageType image_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    bool create_view,
    VkImageAspectFlags view_aspect_flags,
    vk_image* out_image
);


void vk_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vk_image* image,
    VkImageAspectFlags view_aspect_flags
);

void vk_image_destroy(vulkan_context* context, vk_image* image);
