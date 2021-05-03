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
    VkPhysicalDevice physical_device;
    VkDevice device;

    u32 queue_index;
    VkQueue queue;
    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VkSemaphore render_semaphore;
} _Mirai_Gfx;

typedef struct _MG_Swapchain {
    VkSurfaceKHR surface;
    VkFormat format;
    VkColorSpaceKHR color_space;
    u32 width, height;
    VkSwapchainKHR handle;
    VkImageView image_views[2];
    VkSemaphore present_semaphore;
    u32 index;
} _MG_Swapchain;

typedef struct _MG_Pass {
    VkRenderPass handle;
    VkFramebuffer framebuffers[2];
} _MG_Pass;

typedef struct _MG_Pipeline {
    VkShaderModule vs_shader;
    VkShaderModule fs_shader;

    VkPipelineLayout layout;
    VkPipeline handle;
} _MG_Pipeline;

Mirai_Gfx
mg_create()
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
    {
        VkPhysicalDevice physical_devices[4] = {0};
        u32 devices_count = sizeof(physical_devices) / sizeof(physical_devices[0]);

        VkResult res = vkEnumeratePhysicalDevices(
            gfx->instance, &devices_count, physical_devices);
        MC_ASSERT(res == VK_SUCCESS);

        gfx->physical_device = physical_devices[0];

        VkPhysicalDeviceProperties properties = {0};
        vkGetPhysicalDeviceProperties(gfx->physical_device, &properties);

        MC_INFO("Physical Device: %s", properties.deviceName);
    }

    // family queue
    {
        VkQueueFamilyProperties queue_properties[4] = {0};
        u32 queue_properties_count = sizeof(queue_properties) / sizeof(queue_properties[0]);

        vkGetPhysicalDeviceQueueFamilyProperties(
            gfx->physical_device,
            &queue_properties_count,
            queue_properties
        );

        MC_ASSERT(queue_properties[gfx->queue_index].queueFlags & VK_QUEUE_GRAPHICS_BIT);
    }

    // command queue
    {
        float queue_priority = 1.0f;

        VkDeviceQueueCreateInfo queue_create_info = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = gfx->queue_index,
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
            gfx->physical_device, &create_info, VK_NULL_HANDLE, &gfx->device);
        MC_ASSERT(res == VK_SUCCESS);

        vkGetDeviceQueue(gfx->device, gfx->queue_index, gfx->queue_index, &gfx->queue);
    }

    // create command pool
    {
        VkCommandPoolCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = gfx->queue_index,
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

    // semaphores
    {
        VkSemaphoreCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        VkResult res = vkCreateSemaphore(
            gfx->device, &create_info, VK_NULL_HANDLE, &gfx->render_semaphore);
        MC_ASSERT(res == VK_SUCCESS);
    }

    return gfx;
}

void
mg_destroy(Mirai_Gfx gfx)
{
    vkDestroySemaphore(gfx->device, gfx->render_semaphore, VK_NULL_HANDLE);

    vkFreeCommandBuffers(gfx->device, gfx->command_pool, 1, &gfx->command_buffer);
    vkDestroyCommandPool(gfx->device, gfx->command_pool, VK_NULL_HANDLE);

    vkDestroyDevice(gfx->device, VK_NULL_HANDLE);
    vkDestroyInstance(gfx->instance, VK_NULL_HANDLE);

    free(gfx);
}

MG_Swapchain
mg_swapchain_create(Mirai_Gfx gfx, void *window_handle)
{
    MG_Swapchain swapchain = (MG_Swapchain)malloc(sizeof(_MG_Swapchain));
    memset(swapchain, 0, sizeof(_MG_Swapchain));

    // create surface
    {
        #if defined(MIRAI_PLATFORM_WINOS)
            VkWin32SurfaceCreateInfoKHR create_info = {
                .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                .hwnd = window_handle,
            };

            VkResult res = vkCreateWin32SurfaceKHR(
                gfx->instance, &create_info, VK_NULL_HANDLE, &swapchain->surface);
            MC_ASSERT(res == VK_SUCCESS);
        #endif

        VkBool32 present_support = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            gfx->physical_device, gfx->queue_index, swapchain->surface, &present_support);
        MC_ASSERT(present_support == VK_TRUE);
    }

    // swapchain
    {
        VkSurfaceCapabilitiesKHR surface_capabilities = {0};

        VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            gfx->physical_device, swapchain->surface, &surface_capabilities);
        MC_ASSERT(res == VK_SUCCESS);
        MC_ASSERT(surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
        MC_ASSERT(surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);

        VkSurfaceFormatKHR surface_formats[16] = {0};
        u32 surface_formats_count = sizeof(surface_formats) / sizeof(surface_formats[0]);

        res = vkGetPhysicalDeviceSurfaceFormatsKHR(
            gfx->physical_device, swapchain->surface, &surface_formats_count, surface_formats);
        MC_ASSERT(res == VK_SUCCESS);

        swapchain->format = surface_formats[0].format;
        swapchain->color_space = surface_formats[0].colorSpace;
        swapchain->width = surface_capabilities.currentExtent.width;
        swapchain->height = surface_capabilities.currentExtent.height;

        VkSwapchainCreateInfoKHR create_info = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = swapchain->surface,
            .minImageCount = surface_capabilities.minImageCount,
            .imageFormat = swapchain->format,
            .imageColorSpace = swapchain->color_space,
            .imageExtent = {
                .width = swapchain->width,
                .height = swapchain->height
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 1,
            .pQueueFamilyIndices = &gfx->queue_index,
            .preTransform = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = VK_TRUE
        };

        res = vkCreateSwapchainKHR(
            gfx->device, &create_info, VK_NULL_HANDLE, &swapchain->handle);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // image views
    {
        VkImage swapchain_images[16] = {0};
        u32 swapchain_images_count = 0;
        VkResult res = vkGetSwapchainImagesKHR(
            gfx->device, swapchain->handle, &swapchain_images_count, VK_NULL_HANDLE);

        u32 image_views_count = sizeof(swapchain->image_views) / sizeof(swapchain->image_views[0]);

        res = vkGetSwapchainImagesKHR(
            gfx->device, swapchain->handle, &swapchain_images_count, swapchain_images);
        MC_ASSERT(res == VK_SUCCESS);
        MC_ASSERT(swapchain_images_count == image_views_count);

        for (u32 i = 0; i < swapchain_images_count; ++i)
        {
            VkImageViewCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = swapchain_images[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchain->format,
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
                gfx->device, &create_info, VK_NULL_HANDLE, &swapchain->image_views[i]);
            MC_ASSERT(res == VK_SUCCESS);
        }
    }

    // semaphore
    {
        VkSemaphoreCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        VkResult res = vkCreateSemaphore(
            gfx->device, &create_info, VK_NULL_HANDLE, &swapchain->present_semaphore);
        MC_ASSERT(res == VK_SUCCESS);
    }

    return swapchain;
}

void
mg_swapchain_destroy(Mirai_Gfx gfx, MG_Swapchain swapchain)
{
    vkDestroySemaphore(gfx->device, swapchain->present_semaphore, VK_NULL_HANDLE);

    u32 image_view_count = sizeof(swapchain->image_views) / sizeof(swapchain->image_views[0]);
    for (u32 i = 0; i < image_view_count; ++i)
    {
        vkDestroyImageView(gfx->device, swapchain->image_views[i], VK_NULL_HANDLE);
    }

    vkDestroySwapchainKHR(gfx->device, swapchain->handle, VK_NULL_HANDLE);
    vkDestroySurfaceKHR(gfx->instance, swapchain->surface, VK_NULL_HANDLE);

    free(swapchain);
}

void
mg_swapchain_present(Mirai_Gfx gfx, MG_Swapchain swapchain)
{
    // present
    {
        VkPresentInfoKHR present_info = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .swapchainCount = 1,
            .pSwapchains = &swapchain->handle,
            .pImageIndices = &swapchain->index,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &gfx->render_semaphore
        };

        VkResult res = vkQueuePresentKHR(gfx->queue, &present_info);
        MC_ASSERT(res == VK_SUCCESS);
    }
    vkQueueWaitIdle(gfx->queue);
}

MG_Pass
mg_pass_create(Mirai_Gfx gfx, MG_Swapchain swapchain)
{
    MG_Pass pass = (MG_Pass)malloc(sizeof(_MG_Pass));
    memset(pass, 0, sizeof(_MG_Pass));

    // render pass
    {
        VkAttachmentDescription attachment = {
            .format = swapchain->format,
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
            gfx->device, &create_info, VK_NULL_HANDLE, &pass->handle);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // framebuffers
    {
        u32 framebuffers_count = sizeof(pass->framebuffers) / sizeof(pass->framebuffers[0]);
        for (u32 i = 0; i < framebuffers_count; ++i)
        {
            VkFramebufferCreateInfo create_info = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .renderPass = pass->handle,
                .attachmentCount = 1,
                .pAttachments = &swapchain->image_views[i],
                .width = swapchain->width,
                .height = swapchain->height,
                .layers = 1
            };
            VkResult res = vkCreateFramebuffer(
                gfx->device, &create_info, VK_NULL_HANDLE, &pass->framebuffers[i]);
            MC_ASSERT(res == VK_SUCCESS);
        }
    }

    return pass;
}

void
mg_pass_destroy(Mirai_Gfx gfx, MG_Pass pass)
{
    u32 framebuffers_count = sizeof(pass->framebuffers) / sizeof(pass->framebuffers[0]);
    for (u32 i = 0; i < framebuffers_count; ++i)
    {
        vkDestroyFramebuffer(gfx->device, pass->framebuffers[i], VK_NULL_HANDLE);
    }

    vkDestroyRenderPass(gfx->device, pass->handle, VK_NULL_HANDLE);

    free(pass);
}

void
mg_pass_begin(Mirai_Gfx gfx, MG_Pass pass, MG_Swapchain swapchain)
{
    VkResult res = vkAcquireNextImageKHR(
        gfx->device,
        swapchain->handle,
        U64_MAX,
        swapchain->present_semaphore,
        VK_NULL_HANDLE,
        &swapchain->index);
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
            .renderPass = pass->handle,
            .renderArea.extent = {
                .width = swapchain->width,
                .height = swapchain->height
            },
            .framebuffer = pass->framebuffers[swapchain->index],
            .clearValueCount = 1,
            .pClearValues = &clear_value
        };

        vkCmdBeginRenderPass(gfx->command_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // set viewport and scissor
    {
        VkViewport viewport = {
            .width = swapchain->width,
            .height = swapchain->height,
            .maxDepth = 1.0f
        };

        vkCmdSetViewport(gfx->command_buffer, 0, 1, &viewport);

        VkRect2D scissor = {
            .extent = {
                .width = swapchain->width,
                .height = swapchain->height
            }
        };

        vkCmdSetScissor(gfx->command_buffer, 0, 1, &scissor);
    }
}
void
mg_pass_end(Mirai_Gfx gfx, MG_Pass pass, MG_Swapchain swapchain)
{
    // ignore
    MC_ASSERT(pass);

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
            .pWaitSemaphores = &swapchain->present_semaphore,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &gfx->render_semaphore
        };

        VkResult res = vkQueueSubmit(gfx->queue, 1, &submit_info, VK_NULL_HANDLE);
        MC_ASSERT(res == VK_SUCCESS);
    }
}

void
mg_pass_draw(Mirai_Gfx gfx, MG_Pass pass, MG_Pipeline pipeline, u32 vertex_count)
{
    // ignore
    MC_ASSERT(pass);

    vkCmdBindPipeline(gfx->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->handle);
    vkCmdDraw(gfx->command_buffer, vertex_count, 1, 0, 0);
}

MG_Pipeline
mg_pipeline_create(Mirai_Gfx gfx, MG_Pipeline_Desc desc)
{
    MG_Pipeline pipeline = (MG_Pipeline)malloc(sizeof(_MG_Pipeline));
    memset(pipeline, 0, sizeof(_MG_Pipeline));

    // vertex shaders
    {
        VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = desc.vs_code_size,
            .pCode = (u32 *)desc.vs_code
        };

        VkResult res = vkCreateShaderModule(
            gfx->device, &create_info, VK_NULL_HANDLE, &pipeline->vs_shader);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // fragment shaders
    {
        VkShaderModuleCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = desc.fs_code_size,
            .pCode = (u32 *)desc.fs_code
        };

        VkResult res = vkCreateShaderModule(
            gfx->device, &create_info, VK_NULL_HANDLE, &pipeline->fs_shader);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // pipeline layout
    {
        VkPipelineLayoutCreateInfo create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        };

        VkResult res= vkCreatePipelineLayout(
            gfx->device, &create_info, VK_NULL_HANDLE, &pipeline->layout);
        MC_ASSERT(res == VK_SUCCESS);
    }

    // pipeline
    {
        VkPipelineShaderStageCreateInfo stages[] = {
            [0] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = pipeline->vs_shader,
                .pName = "main"
            },
            [1] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = pipeline->fs_shader,
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

        VkRect2D scissor = {0};

        VkPipelineViewportStateCreateInfo viewport_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount = 1,
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

        VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = sizeof(dynamic_states) / sizeof(dynamic_states[0]),
            .pDynamicStates = dynamic_states
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
            .layout = pipeline->layout,
            .renderPass = desc.pass->handle,
            .pDynamicState = &dynamic_state
        };

        VkResult res = vkCreateGraphicsPipelines(
            gfx->device, VK_NULL_HANDLE, 1, &create_info, VK_NULL_HANDLE, &pipeline->handle);
        MC_ASSERT(res == VK_SUCCESS);
    }

    return pipeline;
}

void
mg_pipeline_destroy(Mirai_Gfx gfx, MG_Pipeline pipeline)
{
    vkDestroyPipeline(gfx->device, pipeline->handle, VK_NULL_HANDLE);
    vkDestroyPipelineLayout(gfx->device, pipeline->layout, VK_NULL_HANDLE);

    vkDestroyShaderModule(gfx->device, pipeline->fs_shader, VK_NULL_HANDLE);
    vkDestroyShaderModule(gfx->device, pipeline->vs_shader, VK_NULL_HANDLE);

    free(pipeline);
}
