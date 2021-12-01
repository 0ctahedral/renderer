#include <stdio.h>
#include "vk_device.h"
#include "containers/array.h"

/// struct for storing information about a given
/// physical device that we are querying
/// meant to be temporary
typedef struct vk_physical_device_requirements {
    // does it have these queues?
    bool graphics;
    bool present;
    bool compute;
    bool transfer;

    // aray
    const char ** device_ext_names;
    // ???
    bool sampler_anisotropy;
    bool descrete_gpu;

} vk_physical_device_requirements;

/// stores the indicies of a device
typedef struct vk_physical_device_queue_family_info {
    u32 graphics;
    u32 present;
    u32 compute;
    u32 transfer;
} vk_physical_device_queue_family_info;

typedef struct vk_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* formats;
    u32 present_mode_count;
    VkPresentModeKHR* present_modes;
} vk_swapchain_support_info;

bool select_physical_device(vulkan_context* context);

bool vk_device_create(vulkan_context* context) {
    return select_physical_device(context);
}

void vk_device_destroy(vulkan_context* context) {

}

bool physical_devices_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    VkPhysicalDeviceProperties properties,
    VkPhysicalDeviceFeatures features,
    vk_physical_device_requirements requirements,
    vk_physical_device_queue_family_info* out_queue_info,
    vk_swapchain_support_info* out_support_info
);

bool select_physical_device(vulkan_context* context) {
    // get the number of devices
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(
                context->instance,
                &physical_device_count,
                0
    ));

    if (physical_device_count == 0) {
        // no devices found
        return false;
    }

    // allocate an array and fill it
    VkPhysicalDevice* physical_devices =
        array_reserve(physical_device_count, VkPhysicalDevice);
    VK_CHECK(vkEnumeratePhysicalDevices(
                context->instance,
                &physical_device_count,
                physical_devices
    ));

    // loop over devices and get properties
    for (u32 i = 0; i < physical_device_count; ++i) {
        // properties
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        // mem properties
        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory);

        // TODO: figure this out
        bool supports_device_local_host_visible = false;
        for (u32 i = 0; i < memory.memoryTypeCount; ++i) {
            if (
                ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) &&
                ((memory.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)) {
                supports_device_local_host_visible = true;
                break;
            }
        }

        // features
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);


        vk_physical_device_requirements reqs = {};
        reqs.graphics = true;
        reqs.present = true;
        reqs.transfer = true;
        // reqs.compute = true;
        reqs.sampler_anisotropy = true;
        // TODO: may be necessary
        //reqs.descrete_gpu = true;
        reqs.descrete_gpu = false;

        // we need at least the swapchain extension
        reqs.device_ext_names = array_create(const char*);
        array_push(reqs.device_ext_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        // check if this device meets the requirements
        vk_physical_device_queue_family_info queue_info = {};
        vk_swapchain_support_info support_info = {};
        bool meets_requirements = physical_devices_meets_requirements(
            physical_devices[i],
            context->surface,
            properties,
            features,
            reqs,
            &queue_info,
            &support_info
        );

        if (!meets_requirements) {
            fprintf(stderr, "device %d does not meet requirements", i);
        }
    }

    array_destroy(physical_devices);

    return true;
}

/// Returns if the device given meets requirements set
/// in the requirements struct.
/// also populates the queue info if possible
bool physical_devices_meets_requirements(
    VkPhysicalDevice device,
    VkSurfaceKHR surface,
    VkPhysicalDeviceProperties properties,
    VkPhysicalDeviceFeatures features,
    vk_physical_device_requirements reqs,
    vk_physical_device_queue_family_info* out_queue_info,
    vk_swapchain_support_info* out_support_info
) {
    out_queue_info->graphics = -1;
    out_queue_info->present = -1;
    out_queue_info->compute = -1;
    out_queue_info->transfer = -1;

    
    // stop here if we need a descrete_gpu but ain't got one
    if (reqs.descrete_gpu) {
        if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            fprintf(stderr, "discrete gpu not found");
            return false;
        }
    }

    // get the queue families
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, 0);

    VkQueueFamilyProperties queue_families[32];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);

    for (u32 i = 0; i < queue_family_count; ++i) {

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            out_queue_info->graphics = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            // TODO: make this try to get a dedicated transfer queue
            out_queue_info->transfer = i;
        }

        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_info->compute = i;
        }

        VkBool32 present = VK_FALSE;
        VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present));

        if (present) {
            out_queue_info->present = i;
        }
    }
    // TODO: log collected queue info

    // check if we meet all requirements so far
    if (
        (reqs.graphics && out_queue_info->graphics == -1) ||
        (reqs.compute && out_queue_info->compute == -1) ||
        (reqs.transfer && out_queue_info->transfer == -1) ||
        (reqs.present && out_queue_info->present == -1)
    ) {
        return false;
    }

    // check if we have swapchain support
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        device,
        surface,
        &out_support_info->capabilities
    ));

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
        device,
        surface,
        &out_support_info->format_count,
        0
    ));

    if (out_support_info->format_count != 0) {
        if (!out_support_info->formats) {
            out_support_info->formats = array_reserve(
                out_support_info->format_count,
                VkSurfaceFormatKHR);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(
            device,
            surface,
            &out_support_info->format_count,
            out_support_info->formats));

    }


    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
        device,
        surface,
        &out_support_info->present_mode_count,
        0));

    if (out_support_info->present_mode_count != 0) {
        if (!out_support_info->present_modes) {
            out_support_info->present_modes = array_reserve(
                out_support_info->present_mode_count,
                VkPresentModeKHR);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface,
            &out_support_info->present_mode_count,
            out_support_info->present_modes));

    }

    fprintf(stdout, "# present modes: %d\n", out_support_info->present_mode_count);

    // TODO: Free these bad boys

    // check if we have device extensions

    return true;
}
