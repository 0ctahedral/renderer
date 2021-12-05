#include <stdio.h>
#include "vk_image.h"

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
){
    out_image->width = width;
    out_image->height = height;

    VkImageCreateInfo img_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .extent.width = width,
        .extent.height = height,
        .extent.depth = 1, // TODO
        .mipLevels = 4, // TODO
        .arrayLayers = 1, // TODO
        .format = format,
        .tiling = tiling,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .usage = usage,
        .samples = VK_SAMPLE_COUNT_1_BIT, // TODO
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE, // TODO
    };

    VK_CHECK(vkCreateImage(
        context->device.logical_device,
        &img_info,
        context->allocator,
        &out_image->handle
    ));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(
        context->device.logical_device,
        out_image->handle,
        &memory_requirements
    );

    i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits, memory_flags);
    if (memory_type == -1) {
        fprintf(stderr, "required memory type not found\n");
        VK_CHECK(false);
    }

    VkMemoryAllocateInfo memory_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memory_requirements.size,
        .memoryTypeIndex = memory_type
    };
    VK_CHECK(vkAllocateMemory(
        context->device.logical_device,
        &memory_allocate_info,
        context->allocator,
        &out_image->memory
    ));


    VK_CHECK(vkBindImageMemory(
        context->device.logical_device,
        out_image->handle,
        out_image->memory,
        0 // TODO: configure memory offset
    ));

    if (create_view) {
        out_image->view = 0;
        vk_image_view_create(context, format, out_image, view_aspect_flags);
    }
}


void vk_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vk_image* image,
    VkImageAspectFlags aspect_flags
){
    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image->handle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,  // TODO: Make configurable.
        .format = format,
        .subresourceRange.aspectMask = aspect_flags,

        // TODO: Make configurable
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
    };

    VK_CHECK(vkCreateImageView(context->device.logical_device, &view_create_info, context->allocator, &image->view));
}

void vk_image_destroy(vulkan_context* context, vk_image* image){
    if (image->view) {
        vkDestroyImageView(context->device.logical_device, image->view, context->allocator);
        image->view = 0;
    }
    if (image->memory) {
        vkFreeMemory(context->device.logical_device, image->memory, context->allocator);
        image->memory = 0;
    }
    if (image->handle) {
        vkDestroyImage(context->device.logical_device, image->handle, context->allocator);
        image->handle = 0;
    }
}
