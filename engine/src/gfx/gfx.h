#pragma once

#include "defines.h"

typedef struct MG_Swapchain MG_Swapchain;

MIRAI_API b8 mg_create();
MIRAI_API void mg_destroy();

MIRAI_API MG_Swapchain * mg_swapchain_create();
MIRAI_API void mg_swapchain_destroy(MG_Swapchain *swapchain);
MIRAI_API void mg_swapchain_present(MG_Swapchain *swapchain);
