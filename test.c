#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "renderer/renderer.h"
#include <stdio.h>

int main() {
  if (!glfwInit()) {
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow *window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

  renderer_backend renderer = {};

  renderer_backend_create(&renderer);
  renderer.window = window;

  renderer.initialize(&renderer, "name");

  if (!window) {
    glfwTerminate();
    return 1;
  }


  while (!glfwWindowShouldClose(window)) {
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  renderer.shutdown(&renderer);

  renderer_backend_destroy(&renderer);

  glfwDestroyWindow(window);
  glfwTerminate();
}
