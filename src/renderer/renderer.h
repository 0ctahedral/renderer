#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdbool.h>
// this will be the renderer api

/// open a window
GLFWwindow* open_window();

// run main loop
int main_loop();


// initialize the renderer
bool initialize(const char *name);
