#pragma once

#include "defines.h"

typedef struct _Mirai_Gfx *Mirai_Gfx;
typedef struct _MG_Swapchain *MG_Swapchain;
typedef struct _MG_Pass *MG_Pass;
typedef struct _MG_Pipeline *MG_Pipeline;

MIRAI_API Mirai_Gfx mg_create();
MIRAI_API void mg_destroy(Mirai_Gfx gfx);

MIRAI_API MG_Swapchain mg_swapchain_create(Mirai_Gfx gfx, void *window_handle);
MIRAI_API void mg_swapchain_destroy(Mirai_Gfx gfx, MG_Swapchain swapchain);
MIRAI_API void mg_swapchain_present(Mirai_Gfx gfx, MG_Swapchain swapchain);

MIRAI_API MG_Pass mg_pass_create(Mirai_Gfx gfx, MG_Swapchain swapchain);
MIRAI_API void mg_pass_destroy(Mirai_Gfx gfx, MG_Pass pass);
typedef struct MG_Pipeline_Desc {
    MG_Pass pass;
    const char *vs_code;
    u64 vs_code_size;
    const char *fs_code;
    u64 fs_code_size;
} MG_Pipeline_Desc;

MIRAI_API MG_Pipeline mg_pipeline_create(Mirai_Gfx gfx, MG_Pipeline_Desc desc);
MIRAI_API void mg_pipeline_destroy(Mirai_Gfx gfx, MG_Pipeline pipeline);

MIRAI_API void mg_pass_begin(Mirai_Gfx gfx, MG_Pass pass, MG_Swapchain MG_Swapchain);
MIRAI_API void mg_pass_end(Mirai_Gfx gfx, MG_Pass pass, MG_Swapchain MG_Swapchain);
MIRAI_API void mg_pass_draw(Mirai_Gfx gfx, MG_Pass pass, MG_Pipeline pipeline, u32 vertex_count);
