#include "renderer.h"
#include "stdlib.h"
#include "string.h"

static vulkan_context context;

const uint32_t required_validation_layer_count = 1;
const char* required_validation_layer_names[1] = {"VK_LAYER_KHRONOS_validation"};

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

  initialize("octal");

  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  glfwDestroyWindow(window);
  glfwTerminate();

  shutdown();

  return 0;
}

bool initialize(const char *name) {
    // create app info
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = name;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Octal Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;

    uint32_t count;
    const char** extensions = glfwGetRequiredInstanceExtensions(&count);

    create_info.enabledExtensionCount = count;
    create_info.ppEnabledExtensionNames = extensions;

    // TODO: maybe get rid of this later if we don't need validation
    uint32_t available_layer_count = 0;
    vkEnumerateInstanceLayerProperties(&available_layer_count, 0);
    // TODO: make container type
    VkLayerProperties* available_layers = malloc(sizeof(VkLayerProperties) * available_layer_count);
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
    free(available_layers);

    // add the layers names
    create_info.enabledLayerCount = required_validation_layer_count;
    create_info.ppEnabledLayerNames = required_validation_layer_names;


    // TODO: create debugger


    assertf(vkCreateInstance(&create_info, NULL, &context.instance) == VK_SUCCESS, "failed to create instance");

    return false;
}

void shutdown() {

}
