#include "gfx/gfx.h"
#include "core/asserts.h"
#include "core/logger.h"

#if defined(MIRAI_PLATFORM_WINOS)
    #define VK_USE_PLATFORM_WIN32_KHR
#endif
#include <vulkan/vulkan.h>

#include <stdlib.h>
#include <string.h>

typedef struct _Mirai_Gfx {
    VkInstance instance;
    VkDevice device;

    VkSurfaceKHR surface;
    VkFormat format;
    VkColorSpaceKHR color_space;
    u32 width, height;
    VkSwapchainKHR swapchain;
    VkImageView image_views[2];
    VkFramebuffer framebuffers[2];

    VkRenderPass render_pass;

    VkQueue queue;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VkShaderModule vs_shader;
    VkShaderModule fs_shader;

    VkPipelineLayout pipeline_layout;
    VkPipeline pipeline;

    VkSemaphore present_semaphore;
    VkSemaphore render_semaphore;
} _Mirai_Gfx;


Mirai_Gfx
mg_create(void *window_handle, const char *vs_code, u64 vs_code_size, const char *fs_code, u64 fs_code_size)
{
    Mirai_Gfx gfx = (Mirai_Gfx)malloc(sizeof(_Mirai_Gfx));
    memset(gfx, 0, sizeof(_Mirai_Gfx));

    // instance
    {
        const char *validation_layers[] = {
            "VK_LAYER_KHRONOS_validation"
        };

        const char *instance_extensions[] = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };

        #if MIRAI_DEBUG == 1
            u32 instance_extensions_count = sizeof(instance_extensions) / sizeof(instance_extensions[0]);
        #else
            u32 instance_extensions_count =
                (sizeof(instance_extensions) / sizeof(instance_extensions[0])) - 1;
        #endif

        VkInstanceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            #if MIRAI_DEBUG == 1
                .enabledLayerCount = sizeof(validation_layers) / sizeof(validation_layers[0]),
                .ppEnabledLayerNames = validation_layers,
            #endif
            .enabledExtensionCount = instance_extensions_count,
            .ppEnabledExtensionNames = instance_extensions
        };

        VkResult res = vkCreateInstance(&create_info, VK_NULL_HANDLE, &gfx->instance);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // physical device
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    {
        VkPhysicalDevice physical_devices[4] = {0};
        u32 devices_count = sizeof(physical_devices) / sizeof(physical_devices[0]);

        VkResult res = vkEnumeratePhysicalDevices(
            gfx->instance, &devices_count, physical_devices);
        MC_ASSERT(res == VK_SUCCESS);

        physical_device = physical_devices[0];

        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(physical_device, &properties);

        MC_INFO("Physical Device: %s", properties.deviceName);
    }

    // family queue
    u32 queue_index = 0;
    {
        VkQueueFamilyProperties queue_properties[4] = {0};
        u32 queue_properties_count = sizeof(queue_properties) / sizeof(queue_properties[0]);

        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &queue_properties_count,
            queue_properties
        );

        MC_ASSERT(queue_properties[queue_index].queueFlags & VK_QUEUE_GRAPHICS_BIT);
    }

    // command queue
    {
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = queue_index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority
        };

        const char *device_extensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queue_create_info,
            .enabledExtensionCount = sizeof(device_extensions) / sizeof(device_extensions[0]),
            .ppEnabledExtensionNames = device_extensions
        };

        VkResult res = vkCreateDevice(
            physical_device, &create_info, VK_NULL_HANDLE, &gfx->device);
        MC_ASSERT(res == VK_SUCCESS);

        vkGetDeviceQueue(gfx->device, queue_index, queue_index, &gfx->queue);
    }

    // create command pool
    {
        VkCommandPoolCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = queue_index,
        };

        VkResult res = vkCreateCommandPool(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->command_pool);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // create command buffer
    {
        VkCommandBufferAllocateInfo alloc_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = gfx->command_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkResult res = vkAllocateCommandBuffers(gfx->device, &alloc_info, &gfx->command_buffer);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // create surface
    {
        #if defined(MIRAI_PLATFORM_WINOS)
            VkWin32SurfaceCreateInfoKHR create_info = {
                .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .hwnd = window_handle,
            };

            VkResult res = vkCreateWin32SurfaceKHR(
                gfx->instance, &create_info, VK_NULL_HANDLE, &gfx->surface);
            MC_ASSERT(res == VK_SUCCESS);
        #endif

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            physical_device, queue_index, gfx->surface, &present_support);
        MC_ASSERT(present_support == VK_TRUE);
    }

    // swapchain
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {0};

        VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            physical_device, gfx->surface, &surface_capabilities);
        MC_ASSERT(res == VK_SUCCESS);
        MC_ASSERT(surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
        MC_ASSERT(surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);

        VkSurfaceFormatKHR surface_formats[16] = {0};
        u32 surface_formats_count = sizeof(surface_formats) / sizeof(surface_formats[0]);

        res = vkGetPhysicalDeviceSurfaceFormatsKHR(
            physical_device, gfx->surface, &surface_formats_count, surface_formats);
        MC_ASSERT(res == VK_SUCCESS);

        gfx->format = surface_formats[0].format;
        gfx->color_space = surface_formats[0].colorSpace;
        gfx->width = surface_capabilities.currentExtent.width;
        gfx->height = surface_capabilities.currentExtent.height;

        VkSwapchainCreateInfoKHR create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = gfx->surface,
            .minImageCount = surface_capabilities.minImageCount,
            .imageFormat = gfx->format,
            .imageColorSpace = gfx->color_space,
            .imageExtent = {
                .width = gfx->width,
                .height = gfx->height
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &queue_index,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = VK_TRUE
        };

        res = vkCreateSwapchainKHR(gfx->device, &create_info, VK_NULL_HANDLE, &gfx->swapchain);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // image views
    {
        VkImage swapchain_images[16] = {0};
        u32 swapchain_images_count = 0;
        VkResult res = vkGetSwapchainImagesKHR(
            gfx->device, gfx->swapchain, &swapchain_images_count, VK_NULL_HANDLE);

        u32 image_views_count = sizeof(gfx->image_views) / sizeof(gfx->image_views[0]);

        res = vkGetSwapchainImagesKHR(
            gfx->device, gfx->swapchain, &swapchain_images_count, swapchain_images);
        MC_ASSERT(res == VK_SUCCESS);
        MC_ASSERT(swapchain_images_count == image_views_count);

        for (u32 i = 0; i < swapchain_images_count; ++i)
        {
            VkImageViewCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapchain_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = gfx->format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .levelCount = 1,
                    .layerCount = 1
                }
            };

            VkResult res = vkCreateImageView(
                gfx->device, &create_info, VK_NULL_HANDLE, &gfx->image_views[i]);
            MC_ASSERT(res == VK_SUCCESS);
        }
    }

    // render pass
    {
        VkAttachmentDescription attachment = {
            .format = gfx->format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };

        VkAttachmentReference color_attachment = {
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription supbass = {
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &color_attachment
        };

        VkRenderPassCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &attachment,
            .subpassCount = 1,
            .pSubpasses = &supbass
        };

        VkResult res = vkCreateRenderPass(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->render_pass);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // framebuffers
    {
        u32 framebuffers_count = sizeof(gfx->framebuffers) / sizeof(gfx->framebuffers[0]);
        for (u32 i = 0; i < framebuffers_count; ++i)
        {
            VkFramebufferCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = gfx->render_pass,
                .attachmentCount = 1,
                .pAttachments = &gfx->image_views[i],
                .width = gfx->width,
                .height = gfx->height,
                .layers = 1
            };
            VkResult res = vkCreateFramebuffer(
                gfx->device, &create_info, VK_NULL_HANDLE, &gfx->framebuffers[i]);
            MC_ASSERT(res == VK_SUCCESS);
        }
    }

    // semaphores
    {
        VkSemaphoreCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        VkResult res = vkCreateSemaphore(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->present_semaphore);
        MC_ASSERT(res == VK_SUCCESS);

        res = vkCreateSemaphore(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->render_semaphore);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // vertex shaders
    {
        VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = vs_code_size,
            .pCode = (u32 *)vs_code
        };

        VkResult res = vkCreateShaderModule(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->vs_shader);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // fragment shaders
    {
        VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = fs_code_size,
            .pCode = (u32 *)fs_code
        };

        VkResult res = vkCreateShaderModule(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->fs_shader);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // pipeline layout
    {
        VkPipelineLayoutCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        };

        VkResult res= vkCreatePipelineLayout(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->pipeline_layout);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // pipeline
    {
        VkPipelineShaderStageCreateInfo stages[] = {
            [0] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = gfx->vs_shader,
                .pName = "main"
            },
            [1] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = gfx->fs_shader,
                .pName = "main"
            }
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
        };

        VkViewport viewport = {
            .width = gfx->width,
            .height = gfx->height,
            .maxDepth = 1.0f
        };

        VkRect2D scissor = {
            .extent = {
                .width = gfx->width,
                .height = gfx->height
            }
        };

        VkPipelineViewportStateCreateInfo viewport_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
            .pViewports = &viewport,
            .scissorCount = 1,
            .pScissors = &scissor
        };

        VkPipelineRasterizationStateCreateInfo rasterization_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth = 1.0f
        };

        VkPipelineMultisampleStateCreateInfo multisample_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
        };

        VkPipelineColorBlendAttachmentState attachment = {
            .colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT |
                VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo blend_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = 1,
            .pAttachments = &attachment
        };

        VkGraphicsPipelineCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = sizeof(stages) / sizeof(stages[0]),
            .pStages = stages,
            .pVertexInputState = &vertex_input_state,
            .pInputAssemblyState = &input_assembly_state,
            .pViewportState = &viewport_state,
            .pRasterizationState = &rasterization_state,
            .pMultisampleState = &multisample_state,
            .pColorBlendState = &blend_state,
            .layout = gfx->pipeline_layout,
            .renderPass = gfx->render_pass,
        };

        VkResult res = vkCreateGraphicsPipelines(
            gfx->device, VK_NULL_HANDLE, 1, &create_info, VK_NULL_HANDLE, &gfx->pipeline);
        MC_ASSERT(res == VK_SUCCESS);
    }

    return gfx;
}

void
mg_destroy(Mirai_Gfx gfx)
{
    vkDestroyPipeline(gfx->device, gfx->pipeline, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(gfx->device, gfx->pipeline_layout, VK_NULL_HANDLE);

    vkDestroyShaderModule(gfx->device, gfx->fs_shader, VK_NULL_HANDLE);
    vkDestroyShaderModule(gfx->device, gfx->vs_shader, VK_NULL_HANDLE);

    vkDestroySemaphore(gfx->device, gfx->render_semaphore, VK_NULL_HANDLE);
    vkDestroySemaphore(gfx->device, gfx->present_semaphore, VK_NULL_HANDLE);

    u32 framebuffers_count = sizeof(gfx->framebuffers) / sizeof(gfx->framebuffers[0]);
    for (u32 i = 0; i < framebuffers_count; ++i)
    {
        vkDestroyFramebuffer(gfx->device, gfx->framebuffers[i], VK_NULL_HANDLE);
    }

    u32 image_view_count = sizeof(gfx->image_views) / sizeof(gfx->image_views[0]);
    for (u32 i = 0; i < image_view_count; ++i)
    {
        vkDestroyImageView(gfx->device, gfx->image_views[i], VK_NULL_HANDLE);
    }

    vkDestroyRenderPass(gfx->device, gfx->render_pass, VK_NULL_HANDLE);
    vkDestroySwapchainKHR(gfx->device, gfx->swapchain, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(gfx->instance, gfx->surface, VK_NULL_HANDLE);

    vkFreeCommandBuffers(gfx->device, gfx->command_pool, 1, &gfx->command_buffer);
    vkDestroyCommandPool(gfx->device, gfx->command_pool, VK_NULL_HANDLE);

    vkDestroyDevice(gfx->device, VK_NULL_HANDLE);
    vkDestroyInstance(gfx->instance, VK_NULL_HANDLE);

    free(gfx);
}

void
mg_test_draw(Mirai_Gfx gfx)
{
    u32 image_index = 0;
    VkResult res = vkAcquireNextImageKHR(
        gfx->device, gfx->swapchain, U64_MAX, gfx->present_semaphore, VK_NULL_HANDLE, &image_index);
    MC_ASSERT(res == VK_SUCCESS);

    // begin command buffer
    {
        VkResult res = vkResetCommandBuffer(gfx->command_buffer, 0);
        MC_ASSERT(res == VK_SUCCESS);

        VkCommandBufferBeginInfo begin_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
        };

        res = vkBeginCommandBuffer(gfx->command_buffer, &begin_info);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // begin render pass
    {
        VkClearValue clear_value = {
            .color = {{1.0f, 1.0f, 1.0f, 1.0f}}
        };

        VkRenderPassBeginInfo begin_info = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = gfx->render_pass,
            .renderArea.extent = {
                .width = gfx->width,
                .height = gfx->height
            },
            .framebuffer = gfx->framebuffers[image_index],
            .clearValueCount = 1,
            .pClearValues = &clear_value
        };

        vkCmdBeginRenderPass(gfx->command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // draw triangle
    {
        vkCmdBindPipeline(gfx->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx->pipeline);
        vkCmdDraw(gfx->command_buffer, 3, 1, 0, 0);
    }

    // end render pass
    vkCmdEndRenderPass(gfx->command_buffer);

    // end command buffer
    {
        VkResult res = vkEndCommandBuffer(gfx->command_buffer);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // submit command buffer to command queue
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit_info = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pWaitDstStageMask = &wait_stage,
            .commandBufferCount = 1,
            .pCommandBuffers = &gfx->command_buffer,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &gfx->present_semaphore,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &gfx->render_semaphore
        };

        VkResult res = vkQueueSubmit(gfx->queue, 1, &submit_info, VK_NULL_HANDLE);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // present
    {
        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains = &gfx->swapchain,
            .pImageIndices = &image_index,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &gfx->render_semaphore
        };

        VkResult res = vkQueuePresentKHR(gfx->queue, &present_info);
        MC_ASSERT(res == VK_SUCCESS);
    }

    vkQueueWaitIdle(gfx->queue);
}
