#pragma once

#include "defines.h"

typedef struct _Mirai_Gfx *Mirai_Gfx;

MIRAI_API Mirai_Gfx mg_create(void *window_handle, const char *vs_code, u64 vs_code_size, const char *fs_code, u64 ps_code_size);
MIRAI_API void mg_destroy(Mirai_Gfx gfx);

MIRAI_API void mg_test_draw(Mirai_Gfx gfx);
