#include <stdio.h>
#include "renderer.h"

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

    for (uint32_t i = 0; i < count; ++i) {
        fprintf(stderr, "%s\n", extensions[i]);
    }

    return false;
}
