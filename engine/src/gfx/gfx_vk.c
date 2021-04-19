#include "gfx/gfx.h"
#include "core/asserts.h"
#include "core/logger.h"

#include <vulkan/vulkan.h>

void
mg_init()
{
    u32 extension_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extension_count, NULL);
    MC_DEBUG("%d extenstions supported", extension_count);
}
