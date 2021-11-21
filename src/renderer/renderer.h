#pragma once
#include "renderer_types.inl"

bool renderer_backend_create(renderer_backend_i* out_backend);

void renderer_backend_destroy(renderer_backend_i* backend);
