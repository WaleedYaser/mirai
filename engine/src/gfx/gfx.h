#pragma once

#include "defines.h"

typedef struct _Mirai_Gfx *Mirai_Gfx;

MIRAI_API Mirai_Gfx mg_create(void *window_Handle);
MIRAI_API void mg_destroy(Mirai_Gfx gfx);

MIRAI_API void mg_test_draw(Mirai_Gfx gfx);
