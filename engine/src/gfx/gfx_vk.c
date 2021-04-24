#include "gfx/gfx.h"
#include "core/asserts.h"
#include "core/logger.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

typedef struct _MG_VK_Internal {
    VkInstance instance;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    // TODO: add present queue

    b8 graphics_family_supported;
    u32 graphics_family_index;

    b8 enable_validation_layer;
    VkDebugUtilsMessengerEXT debug_messenger;
} _MG_VK_Internal;

static _MG_VK_Internal *
_mg_vk_internal()
{
    static _MG_VK_Internal gfx;
    return &gfx;
}

static void
_mg_vk_log_available_extensions(void)
{
    u32 extensions_count = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, NULL);

    VkExtensionProperties *available_extensions =
        (VkExtensionProperties *)malloc(extensions_count * sizeof(VkExtensionProperties));
    vkEnumerateInstanceExtensionProperties(NULL, &extensions_count, available_extensions);

    MC_DEBUG("Vulkan available extensions (%u):", extensions_count);
    for (u32 i = 0; i < extensions_count; ++i)
    {
        MC_DEBUG("\t%s", available_extensions[i]);
    }

    free(available_extensions);
}

static void
_mg_vk_log_available_layers(void)
{
    u32 layers_count = 0;
    vkEnumerateInstanceLayerProperties(&layers_count, NULL);

    VkLayerProperties *available_layers =
        (VkLayerProperties *)malloc(layers_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layers_count, available_layers);

    MC_DEBUG("Vulkan available layers (%u):", layers_count);
    for (u32 i = 0; i < layers_count; ++i)
    {
        MC_DEBUG("\t%s", available_layers[i].layerName);
    }

    free(available_layers);
}

static void
_mg_vk_log_physical_devices(VkInstance instance)
{
    u32 devices_count = 0;
    vkEnumeratePhysicalDevices(instance, &devices_count, NULL);
    MC_ASSERT_MSG(devices_count, "Failed to find GPUs with Vulkan support!");

    VkPhysicalDevice *devices = (VkPhysicalDevice *)malloc(devices_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &devices_count, devices);

    MC_DEBUG("Vulkan available devices (%u):", devices_count);
    for (u32 i = 0; i < devices_count; ++i)
    {
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(devices[i], &device_properties);
        MC_DEBUG("\t%s", device_properties.deviceName);
    }

    free(devices);
}

static b8
_mg_vk_check_validation_layer_support(const char *validation_layers[], u32 validation_layers_count)
{
    u32 layers_count;
    vkEnumerateInstanceLayerProperties(&layers_count, NULL);

    VkLayerProperties *available_layers =
        (VkLayerProperties *)malloc(layers_count * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layers_count, available_layers);

    for (u32 i = 0; i < validation_layers_count; ++i)
    {
        b8 layer_found = FALSE;
        for (u32 j = 0; j < layers_count; ++j)
        {
            if (strcmp(validation_layers[i], available_layers[j].layerName) == 0)
            {
                layer_found = TRUE;
                break;
            }
        }

        if (layer_found == FALSE)
        {
            free(available_layers);
            return FALSE;
        }
    }

    free(available_layers);
    return TRUE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
_mg_vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *call_back_data,
    void *user_data)
{
    // TODO: unused parameters
    switch (message_severity){default: break;};
    switch (message_type){default: break;};
    if (user_data){}

    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        MC_ERROR("vulkan validation layer: %s", call_back_data->pMessage);
    }
    else
    {
        MC_TRACE("vulkan validation layer: %s", call_back_data->pMessage);
    }

    return VK_FALSE;
}

b8
mg_create()
{
    _MG_VK_Internal *gfx = _mg_vk_internal();

    _mg_vk_log_available_extensions();
    _mg_vk_log_available_layers();

    #if defined(MIRAI_DEBUG)
        gfx->enable_validation_layer = TRUE;
    #else
        gfx->enable_validation_layer = FALSE;
    #endif

    const char *validation_layers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    const u32 validation_layers_count = sizeof(validation_layers) / sizeof(const char *);

    if (gfx->enable_validation_layer &&
        _mg_vk_check_validation_layer_support(validation_layers, validation_layers_count) == FALSE)
    {
        MC_ASSERT_MSG(FALSE, "couldn't find requested validation layers");
    }

    // optional struct that provides some useful information to the driver in order to optimize our
    // specific application
    VkApplicationInfo app_info = {0};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Mirai Gfx Vulkan";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "No Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;

    const char *enabled_extensions[3] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    };

    // populate debug_messenger_create_info
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info = {0};
    debug_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_create_info.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | // diagnostic messages
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_create_info.messageType =
        // unrelated to specification or performance
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        // violate specification or indicates a possible mistake
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        // potential non-optimal use of Vulkan
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_create_info.pfnUserCallback = _mg_vk_debug_callback;

    // create vulkan instance
    {
        VkInstanceCreateInfo create_info = {0};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = 2;
        create_info.ppEnabledExtensionNames = enabled_extensions;
        if (gfx->enable_validation_layer)
        {
            create_info.enabledExtensionCount = 3;
            create_info.enabledLayerCount = validation_layers_count;
            create_info.ppEnabledLayerNames = validation_layers;
            // to debug instance creation and destruction as vkCreateDebugUtilsMessengerEXT
            // requires a valid instance so we create a separate debug utils messenger specifically
            // for creation and destruction of the instance
            create_info.pNext = &debug_messenger_create_info;
        }
        VkResult result = vkCreateInstance(&create_info, NULL, &gfx->instance);
        MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create vulkan instance");
    }

    // create vulkan debug messenger
    if (gfx->enable_validation_layer)
    {
        // load vkCreateDebugUtilsMessengerEXT function
        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT =
            (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                gfx->instance, "vkCreateDebugUtilsMessengerEXT");
        MC_ASSERT_MSG(vkCreateDebugUtilsMessengerEXT, "Failed to load vkCreateDebugUtilsMessengerEXT");

        // create debug messenger
        VkResult result = vkCreateDebugUtilsMessengerEXT(
            gfx->instance, &debug_messenger_create_info, NULL, &gfx->debug_messenger);
        MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create vulkan debug messenger");
    }

    _mg_vk_log_physical_devices(gfx->instance);

    // select physical device
    {
        gfx->physical_device = VK_NULL_HANDLE;

        u32 devices_count = 0;
        vkEnumeratePhysicalDevices(gfx->instance, &devices_count, NULL);
        MC_ASSERT_MSG(devices_count, "Failed to find GPUs with Vulkan support!");

        VkPhysicalDevice *devices = (VkPhysicalDevice *)malloc(devices_count * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(gfx->instance, &devices_count, devices);

        for (u32 i = 0; i < devices_count; ++i)
        {
            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(devices[i], &device_properties);

            VkPhysicalDeviceFeatures device_features;
            vkGetPhysicalDeviceFeatures(devices[i], &device_features);

            if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                device_features.geometryShader)
            {
                gfx->physical_device = devices[i];
            }
        }
        free(devices);

        MC_ASSERT_MSG(gfx->physical_device != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
        VkPhysicalDeviceProperties selected_device_properties;
        vkGetPhysicalDeviceProperties(gfx->physical_device, &selected_device_properties);
        MC_INFO("selected physical device: %s", selected_device_properties.deviceName);
    }

    // find queue families
    {
        u32 queue_families_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gfx->physical_device, &queue_families_count, NULL);

        VkQueueFamilyProperties *queue_families =
            (VkQueueFamilyProperties *)malloc(queue_families_count * sizeof(VkQueueFamilyProperties));

        for (u32 i = 0; i < queue_families_count; ++i)
        {
            if (queue_families[i].queueFlags && VK_QUEUE_GRAPHICS_BIT)
            {
                gfx->graphics_family_supported = TRUE;
                gfx->graphics_family_index = i;
            }
        }
        free(queue_families);
        MC_ASSERT_MSG(gfx->graphics_family_supported, "Failed to find supported graphics queue family");
    }

    // create logical device
    {
        const char *device_extensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        u32 device_extensions_count = sizeof(device_extensions) / sizeof(const char *);

        f32 queue_priority = 1.0f;
        VkDeviceQueueCreateInfo queue_create_info = {0};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = gfx->graphics_family_index;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;

        VkPhysicalDeviceFeatures device_features = {0};

        VkDeviceCreateInfo create_info = {0};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.pQueueCreateInfos = &queue_create_info;
        create_info.queueCreateInfoCount = 1;
        create_info.pEnabledFeatures = &device_features;
        if (gfx->enable_validation_layer)
        {
            create_info.enabledLayerCount = validation_layers_count;
            create_info.ppEnabledLayerNames = validation_layers;
        }
        create_info.enabledExtensionCount = device_extensions_count;
        create_info.ppEnabledExtensionNames = device_extensions;
        VkResult result = vkCreateDevice(gfx->physical_device, &create_info, NULL, &gfx->device);
        MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create logical device");
    }

    // retrieve queue handles
    {
        vkGetDeviceQueue(gfx->device, gfx->graphics_family_index, 0, &gfx->graphics_queue);
    }

    return TRUE;
}

void
mg_destroy()
{
    _MG_VK_Internal *gfx = _mg_vk_internal();

    vkDestroyDevice(gfx->device, NULL);

    // destroy vulkan debug messenger
    if (gfx->enable_validation_layer)
    {
        // load vkDestroyDebugUtilsMessenger function
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                gfx->instance, "vkDestroyDebugUtilsMessengerEXT");
        MC_ASSERT_MSG(vkDestroyDebugUtilsMessengerEXT, "Failed to load vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(gfx->instance, gfx->debug_messenger, NULL);
    }

    vkDestroyInstance(gfx->instance, NULL);
}

typedef struct MG_Swapchain {
    VkSurfaceKHR surface;
    VkSwapchainKHR handle;

    u32 images_count;
    VkImage images[2];
    VkImageView views[2];

    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
} MG_Swapchain;

MG_Swapchain *
mg_swapchain_create(void *window_handle)
{
    _MG_VK_Internal *gfx = _mg_vk_internal();
    MG_Swapchain *swapchain = (MG_Swapchain *)malloc(sizeof(MG_Swapchain));

    // create window surface
    {
        #if defined(MIRAI_PLATFORM_WINOS)
            VkWin32SurfaceCreateInfoKHR create_info = {
                .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .hwnd = window_handle
            };
            VkResult result = vkCreateWin32SurfaceKHR(
                gfx->instance, &create_info, NULL, &swapchain->surface);
            MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create window surface");
        #else
            #error "unsupported platform"
        #endif
    }
    // TODO: check if different queue support presentation
    // query for presentation support and create presentation queue
    {
        VkBool32 present_support = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            gfx->physical_device,
            gfx->graphics_family_index,
            swapchain->surface,
            &present_support);
        MC_ASSERT_MSG(present_support, "Graphics queue doesn't support present")
    }

    // create swapchain
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            gfx->physical_device, swapchain->surface, &capabilities);

        VkSwapchainCreateInfoKHR create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = swapchain->surface,
            .minImageCount = capabilities.minImageCount,
            .imageFormat = VK_FORMAT_B8G8R8A8_SRGB,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = capabilities.currentExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = VK_TRUE
        };

        VkResult result = vkCreateSwapchainKHR(gfx->device, &create_info, NULL, &swapchain->handle);
        MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create swapchain");
    }

    // retrieve swapchain images
    {
        swapchain->images_count = 0;
        vkGetSwapchainImagesKHR(gfx->device, swapchain->handle, &swapchain->images_count, NULL);
        MC_ASSERT(swapchain->images_count == 2);
        vkGetSwapchainImagesKHR(
            gfx->device, swapchain->handle, &swapchain->images_count, swapchain->images);
    }

    // create image views
    {
        for (u32 i = 0; i < swapchain->images_count; ++i)
        {
            VkImageViewCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapchain->images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = VK_FORMAT_B8G8R8A8_SRGB,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount = 1,
                    .layerCount = 1
                },
            };
            VkResult result = vkCreateImageView(gfx->device, &create_info, NULL, &swapchain->views[i]);
            MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create image view");
        }
    }

    // create semaphores
    {
        // image available semaphore
        {
            VkSemaphoreCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
            VkResult result = vkCreateSemaphore(
                gfx->device, &create_info, NULL, &swapchain->image_available_semaphore);
            MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create semaphore");
        }
        // render finished semaphore
        {
            VkSemaphoreCreateInfo create_info = {.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
            VkResult result = vkCreateSemaphore(
                gfx->device, &create_info, NULL, &swapchain->render_finished_semaphore);
            MC_ASSERT_MSG(result == VK_SUCCESS, "Failed to create semaphore");
        }
    }

    return swapchain;
}

void
mg_swapchain_destroy(MG_Swapchain *swapchain)
{
    _MG_VK_Internal *gfx = _mg_vk_internal();

    vkDestroySemaphore(gfx->device, swapchain->render_finished_semaphore, NULL);
    vkDestroySemaphore(gfx->device, swapchain->image_available_semaphore, NULL);

    for (u32 i = 0; i < swapchain->images_count; ++i)
    {
        vkDestroyImageView(gfx->device, swapchain->views[i], NULL);
    }
    vkDestroySwapchainKHR(gfx->device, swapchain->handle, NULL);
    vkDestroySurfaceKHR(gfx->instance, swapchain->surface, NULL);
    free(swapchain);
}

void
mg_swapchain_present(MG_Swapchain *swapchain)
{
    _MG_VK_Internal *gfx = _mg_vk_internal();

    u32 image_index;
    vkAcquireNextImageKHR(
        gfx->device,
        swapchain->handle,
        U64_MAX,
        swapchain->image_available_semaphore,
        VK_NULL_HANDLE,
        &image_index);

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &swapchain->handle,
        .pImageIndices = &image_index
    };
    vkQueuePresentKHR(gfx->graphics_queue, &present_info);
}
