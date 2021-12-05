#include "vk_swapchain.h"
#include "vk_device.h"
#include "vk_image.h"
#include "containers/array.h"
#include <stdio.h>

void create(
    vulkan_context* context,
    u32 width, u32 height,
    vk_swapchain* swapchain) {

    VkExtent2D extent = {width, height};
    swapchain->max_frames = 2;

    bool found = false;
    for (u32 i = 0; i < context->device.swapchain_support.format_count; ++i) {
        VkSurfaceFormatKHR format = context->device.swapchain_support.formats[i];
        if (format.format == VK_FORMAT_B8G8R8_UNORM &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            swapchain->img_format = format;
            found = true;
            break;
        }
    }

    if (!found) {
        fprintf(stderr, "failed to find desired swapchain format\n");
        swapchain->img_format = context->device.swapchain_support.formats[0];
    }

    // default to fifo
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i = 0; i < context->device.swapchain_support.present_mode_count; ++i) {
        VkPresentModeKHR mode = context->device.swapchain_support.present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            fprintf(stderr, "Found mailbox present mode!\n");
            present_mode = mode;
            break;
        }
    }

    vk_query_swapchain_support(
        context->device.physical_device,
        context->surface,
        &context->device.swapchain_support
    );

    if (context->device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX) {
        extent = context->device.swapchain_support.capabilities.currentExtent;
    }

    VkExtent2D min = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_support.capabilities.maxImageExtent;
    extent.width = CLAMP(extent.width, min.width, max.width);
    extent.height = CLAMP(extent.height, min.height, max.height);

    // how many images can we get?
    u32 img_count = context->device.swapchain_support.capabilities.minImageCount + 1;
    if (context->device.swapchain_support.capabilities.maxImageCount > 0 &&
            img_count > context->device.swapchain_support.capabilities.maxImageCount) {
        img_count = context->device.swapchain_support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchain_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = context->surface,
        .minImageCount = img_count,
        .imageFormat = swapchain->img_format.format,
        .imageColorSpace = swapchain->img_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
    };

    // setup indices
    if (context->device.graphics_queue_idx != context->device.present_queue_idx) {
        u32 indices[] = {
            (u32)context->device.graphics_queue_idx,
            (u32)context->device.present_queue_idx,
        };
        swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_info.queueFamilyIndexCount = 2;
        swapchain_info.pQueueFamilyIndices = indices;
    } else {
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.queueFamilyIndexCount = 0;
        swapchain_info.pQueueFamilyIndices = 0;
    }

    swapchain_info.preTransform = context->device.swapchain_support.capabilities.currentTransform;
    swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_info.presentMode = present_mode;
    swapchain_info.clipped = VK_TRUE;
    swapchain_info.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(
        context->device.logical_device,
        &swapchain_info,
        context->allocator,
        &swapchain->handle
    ));

    context->current_frame = 0;
    swapchain->img_count = 0;

    VK_CHECK(vkGetSwapchainImagesKHR(
        context->device.logical_device,
        swapchain->handle,
        &swapchain->img_count,
        0
    ));
    if (!swapchain->images) {
        swapchain->images = array_reserve(swapchain->img_count, VkImage);
    }
    if (!swapchain->views) {
        swapchain->views = array_reserve(swapchain->img_count, VkImageView);
    }
    VK_CHECK(vkGetSwapchainImagesKHR(
        context->device.logical_device,
        swapchain->handle,
        &swapchain->img_count,
        swapchain->images
    ));

    for (u32 i = 0; i < swapchain->img_count; ++i) {
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = swapchain->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->img_format.format,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1,
        };

        VK_CHECK(vkCreateImageView(
            context->device.logical_device,
            &view_info,
            context->allocator,
            &swapchain->views[i]
        ));
    }

    // create the depth image
    if (!vk_detect_depth_format(&context->device)) {
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        fprintf(stderr, "failed to find a format\n");
    }

    vk_image_create(
        context,
        VK_IMAGE_TYPE_2D,
        extent.width,
        extent.height,
        context->device.depth_format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        true,
        VK_IMAGE_ASPECT_DEPTH_BIT,
        &swapchain->depth
    );


    fprintf(stderr, "swapchain creation success!\n");
}

void destroy(vulkan_context* context, vk_swapchain* swapchain) {
    vk_image_destroy(context, &swapchain->depth);

    for (u32 i = 0; i < swapchain->img_count; ++i) {
        vkDestroyImageView(
            context->device.logical_device,
            swapchain->views[i],
            context->allocator
        );
    }

    vkDestroySwapchainKHR(
        context->device.logical_device,
        swapchain->handle,
        context->allocator
    );
}

void vk_swapchain_create(
    vulkan_context* context,
    u32 width,
    u32 height,
    vk_swapchain* out_swapchain
){
    create(context, width, height, out_swapchain);
}


void vk_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vk_swapchain* swapchain
){

    destroy(context, swapchain);
    create(context, width, height, swapchain);
}

void vk_swapchain_destroy(
    vulkan_context* context,
    vk_swapchain* swapchain
){
    destroy(context, swapchain);
}

bool vk_swapchain_next_image_index(
    vulkan_context* context,
    vk_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore img_avail,
    VkFence fence,
    u32* out_idx
){
    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device,
        swapchain->handle,
        timeout_ns,
        img_avail,
        fence,
        out_idx
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // means we need to recreate the swapchain
        vk_swapchain_recreate(
            context,
            context->fb_width,
            context->fb_height,
            swapchain
        );
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "failed to aquire image\n");
        return false;
    }

    return true;
}

void vk_swapchain_present(
    vulkan_context* context,
    vk_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete,
    u32 present_img_idx
){
    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &render_complete,
        .swapchainCount = 1,
        .pSwapchains = &swapchain->handle,
        .pImageIndices = &present_img_idx,
        .pResults = 0
    };
    VkResult result = vkQueuePresentKHR(present_queue, &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // means we need to recreate the swapchain
        vk_swapchain_recreate(
            context,
            context->fb_width,
            context->fb_height,
            swapchain
        );
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "failed to present swapchain\n");
    }
}
