#include "vk_device.h"
#include "containers/array.h"

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

bool select_physical_device(vulkan_context* context);

bool vk_device_create(vulkan_context* context) {

    

    return true;
}

void vk_device_destroy(vulkan_context* context) {

}


bool select_physical_device(vulkan_context* context) {
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

    VkPhysicalDevice* physical_devices =
        array_reserve(physical_device_count, VkPhysicalDevice);

    VK_CHECK(vkEnumeratePhysicalDevices(
                context->instance,
                &physical_device_count,
                physical_devices
    ));

    for (u32 i = 0; i < physical_device_count; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i], &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i], &features);

        VkPhysicalDeviceMemoryProperties memory_properties;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &memory_properties);

        vk_physical_device_requirements reqs = {};
        reqs.graphics = true;
        reqs.present = true;
        reqs.transfer = true;
        // reqs.compute = true;
        reqs.sampler_anisotropy = true;
        reqs.descrete_gpu = true;

        reqs.device_ext_names = array_create(const char*);
        array_push(reqs.device_ext_names, &VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    }

    array_destroy(physical_devices);

    return true;
}
