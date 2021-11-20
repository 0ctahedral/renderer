#pragma once
#include "renderer_types.inl"

bool renderer_backend_create(renderer_backend* out_backend);

void renderer_backend_destroy(renderer_backend* backend);
