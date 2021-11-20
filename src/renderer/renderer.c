#include "renderer.h"
#include "vulkan/vk_renderer.h"

bool renderer_backend_create(renderer_backend* out_backend) {
    out_backend->initialize = vk_initialize;
    out_backend->shutdown = vk_shutdown;

    return true;
}

void renderer_backend_destroy(renderer_backend* backend) {

}
