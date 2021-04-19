#include "gfx/gfx.h"
#include "core/asserts.h"
#include "core/logger.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct _Mirai_Gfx_Internal {
    VkInstance instance;

    u32 available_extensions_count;
    VkExtensionProperties *available_extensions;

    u32 available_layers_count;
    VkLayerProperties *available_layers;
} _Mirai_Gfx_Internal;

static _Mirai_Gfx_Internal *
_mirai_gfx_internal()
{
    static _Mirai_Gfx_Internal gfx;
    return &gfx;
}

b8
mg_create()
{
    _Mirai_Gfx_Internal *gfx = _mirai_gfx_internal();

    // optional struct that provides some useful information to the driver in order to optimize our
    // specific application
    gfx->available_extensions_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &gfx->available_extensions_count, NULL);
    gfx->available_extensions = (VkExtensionProperties *)malloc(
        gfx->available_extensions_count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(
        NULL,
        &gfx->available_extensions_count,
        gfx->available_extensions);

    MC_DEBUG("Vulkan available extensions (%u):", gfx->available_extensions_count);
    for (u32 i = 0; i < gfx->available_extensions_count; ++i)
    {
        MC_DEBUG("\t%s", gfx->available_extensions[i]);
    }

    u32 validation_layers_count = 1;
    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    gfx->available_layers_count = 0;
    vkEnumerateInstanceLayerProperties(&gfx->available_layers_count, NULL);
    gfx->available_layers = (VkLayerProperties *)malloc(
        gfx->available_layers_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&gfx->available_layers_count, gfx->available_layers);
    MC_DEBUG("Vulkan available layers (%u):", gfx->available_layers_count);
    for (u32 i = 0; i < gfx->available_layers_count; ++i)
    {
        MC_DEBUG("\t%s", gfx->available_layers[i].layerName);
    }

    b8 all_layers_found = TRUE;
    for (u32 i = 0; i < validation_layers_count; ++i)
    {
        b8 layer_found = FALSE;
        for (u32 j = 0; j < gfx->available_layers_count; ++j)
        {
            if (strcmp(validation_layers[i], gfx->available_layers[j].layerName) == 0)
            {
                layer_found = TRUE;
                break;
            }
        }

        if (layer_found == FALSE)
        {
            MC_ERROR("couldn't find '%s' validation layer", validation_layers[i]);
            all_layers_found = FALSE;
            break;
        }
    }

    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Mirai Gfx Vulkan";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    const char *enabled_extensions[2] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 2;
    create_info.ppEnabledExtensionNames = enabled_extensions;
    #if MIRAI_DEBUG
        create_info.enabledLayerCount = validation_layers_count;
        create_info.ppEnabledLayerNames = validation_layers;
    #endif

    VkResult result = vkCreateInstance(&create_info, NULL, &gfx->instance);
    MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create vulkan instance");

    return TRUE;
}

void
mg_destroy()
{
    _Mirai_Gfx_Internal *gfx = _mirai_gfx_internal();
    free(gfx->available_extensions);
    vkDestroyInstance(gfx->instance, NULL);
}