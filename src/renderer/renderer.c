#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

#include "../asserts.h"
#include "renderer.h"
#include "../containers/array.h"

static vulkan_context context;

const uint32_t required_validation_layer_count = 1;
const char* required_validation_layer_names[] = {"VK_LAYER_KHRONOS_validation"};
// TODO: do this dynamically, you dink
const char* extensions[] = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
    "VK_KHR_surface",
    "VK_KHR_xcb_surface"
};


VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    // TODO: handle error types
    fprintf(stderr, "vk_msg: %s\n", callback_data->pMessage);
    return VK_FALSE;
}

GLFWwindow *open_window() {
  if (!glfwInit()) {
    return NULL;
  }

  GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
  return window;
}

int main_loop() {
  GLFWwindow *window = open_window();
  if (!window) {
    glfwTerminate();
    return 1;
  }

  initialize_renderer("octal");

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  shutdown_renderer();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

bool initialize_renderer(const char *name) {
    // TODO: make an allocator
    context.allocator = 0;
    // create app info
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Octal Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;

    uint32_t count = 3;
    create_info.enabledExtensionCount = count;
    create_info.ppEnabledExtensionNames = extensions;

    // TODO: maybe get rid of this later if we don't need validation
    uint32_t available_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&available_layer_count, 0);
    // TODO: make container type

    VkLayerProperties* available_layers = array_reserve(
            available_layer_count,
            VkLayerProperties
    );
    vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers);
    for (uint32_t i = 0; i < required_validation_layer_count; ++i) {
        bool found = false;
        for (uint32_t j = 0; j < available_layer_count; ++j) {
            if (strcmp(required_validation_layer_names[i], available_layers[j].layerName) == 0) {
                found = true;
                break;
            }
        }

        if (!found) {
            fprintf(stderr, "could not find validation layer: %s\n", required_validation_layer_names[i]);
            return false;
        }
    }
    // done with the layers
    array_destroy(available_layers);

    // add the layers names
    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;

    // create the instance
    assertf(
            vkCreateInstance(&create_info, context.allocator, &context.instance)
            == VK_SUCCESS, "failed to create instance"
    );

    VkDebugUtilsMessengerCreateInfoEXT debug_info={VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                |VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
                                |VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;

    debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
      |VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_info.pfnUserCallback = vk_debug_callback;

    // get the function to set this bad boy up
    // it's an extension so we need to look it up
    PFN_vkCreateDebugUtilsMessengerEXT func =
      (PFN_vkCreateDebugUtilsMessengerEXT)
      vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");

    assertf(func, "Could not find function 'vkCreateDebugUtilsMessengerEXT'");
    assert(func(
                context.instance,
                &debug_info,
                context.allocator,
                &context.debug_messenger
            ) == VK_SUCCESS);

    return true;
}

void shutdown_renderer(void) {
    PFN_vkDestroyDebugUtilsMessengerEXT func =
        (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");

    assertf(func, "Could not find function 'vkDestroyDebugUtilsMessengerEXT'");

    func(context.instance, context.debug_messenger, context.allocator);

    vkDestroyInstance(context.instance, context.allocator);
}
