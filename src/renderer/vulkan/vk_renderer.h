#pragma once

#include "defines.h"
#include "renderer/renderer_types.inl"
#include "vk_types.inl"

// run main loop
int main_loop();

/// context

/// initialize the renderer
bool vk_initialize(GLFWwindow* window, const char* name);

/// shutdown the renderer
void vk_shutdown();
