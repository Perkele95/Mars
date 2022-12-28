//
// Created by arlev on 21.12.2022.
//

#include "VulkanInstance.hpp"
#include <GLFW/glfw3.h>

namespace vks
{
    void VulkanInstance::initialise(GLFWwindow *window, VkExtent2D screenExtent)
    {
        extent = screenExtent;
        currentFrame = 1;
        imageIndex = 0;
        swapchain = VK_NULL_HANDLE;
        imageCount = 0;
        vSync = false;

        auto appInfo = Inits::applicationInfo("Mars");
        appInfo.engineVersion = MakeVersionU32(1, 0, 0);
        appInfo.applicationVersion = MakeVersionU32(1, 0, 0);

        constexpr const char *RequiredExtensions[] = {
#if defined(_WIN32)
                "VK_KHR_win32_surface",
#endif
                VK_KHR_SURFACE_EXTENSION_NAME
        };

        auto instanceInfo = Inits::instanceCreateInfo();
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = arraysize32(RequiredExtensions);
        instanceInfo.ppEnabledExtensionNames = RequiredExtensions;
#ifdef ENABLE_VALIDATION
        instanceInfo.enabledLayerCount = arraysize32(ValidationLayers);
        instanceInfo.ppEnabledLayerNames = ValidationLayers;
#else
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
#endif
        vkCreateInstance(&instanceInfo, 0, &instance);

        glfwCreateWindowSurface(instance, window, VK_NULL_HANDLE, &surface);

        device.initialise(instance, surface);

        {
            uint32_t count = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device.gpu, surface, &count, nullptr);
            auto surfaceFormats = std::vector<VkSurfaceFormatKHR>(count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device.gpu, surface, &count, surfaceFormats.data());

            surfaceFormat = *surfaceFormats.begin();

            for (const auto &sFormat: surfaceFormats)
            {
                const bool formatCheck = sFormat.format == VK_FORMAT_B8G8R8A8_SRGB;
                const bool colourSpaceCheck = sFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                if (formatCheck && colourSpaceCheck)
                {
                    surfaceFormat = sFormat;
                    break;
                }
            }
        }

        setupSwapchain();

        sampleCount = Tools::SampleCount(device.gpu);
        depth.format = Tools::DepthFormat(device.gpu);
        msaa.format = surfaceFormat.format;

        setupMsaa();
        setupDepth();
        setupRenderPass();
        setupFramebuffers();
        setupSyncPrimitives();

        auto cmdInfo = Inits::commandBufferAllocateInfo(device.commandPool, MAX_IMAGES_IN_FLIGHT);
        vkAllocateCommandBuffers(device, &cmdInfo, commandBuffers);

        buildCommandBuffers();
    }

    void VulkanInstance::shutdown()
    {
        for (size_t i = 0; i < MAX_IMAGES_IN_FLIGHT; i++)
        {
            vkDestroyFence(device, sync.inFlightFences[i], nullptr);
            vkDestroySemaphore(device, sync.renderFinishedSPs[i], nullptr);
            vkDestroySemaphore(device, sync.imageAvailableSPs[i], nullptr);
        }

        vkDestroyRenderPass(device, renderPass, VK_NULL_HANDLE);

        for (auto &framebuffer : framebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

        vkDestroyImage(device, depth.image, nullptr);
        vkDestroyImageView(device, depth.view, nullptr);
        vkFreeMemory(device, depth.memory, nullptr);

        vkDestroyImage(device, msaa.image, nullptr);
        vkDestroyImageView(device, msaa.view, nullptr);
        vkFreeMemory(device, msaa.memory, nullptr);

        for (auto &swapchainView : swapchainViews)
            vkDestroyImageView(device, swapchainView, nullptr);

        vkDestroySwapchainKHR(device, swapchain, nullptr);

        device.shutdown();
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void VulkanInstance::prepareFrame()
    {
        currentFrame = (currentFrame + 1) % MAX_IMAGES_IN_FLIGHT;
        vkQueueWaitIdle(device.graphicsQueue);

        vkWaitForFences(device, 1, &sync.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &sync.inFlightFences[currentFrame]);

        const auto result = vkAcquireNextImageKHR(device,
                                                  swapchain,
                                                  UINT64_MAX,
                                                  sync.imageAvailableSPs[currentFrame],
                                                  VK_NULL_HANDLE,
                                                  &imageIndex);

        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
            windowResize();
            return;
        }
    }

    void VulkanInstance::submitFrame()
    {
        const VkSemaphore imageAvailableSemaphores[] = { sync.imageAvailableSPs[currentFrame] };
        const VkSemaphore renderFinishedSemaphores[] = { sync.renderFinishedSPs[currentFrame] };
        const VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.pCommandBuffers = commandBuffers;
        submitInfo.commandBufferCount = arraysize32(commandBuffers);
        submitInfo.pWaitSemaphores = imageAvailableSemaphores;
        submitInfo.waitSemaphoreCount = arraysize32(imageAvailableSemaphores);
        submitInfo.pSignalSemaphores = renderFinishedSemaphores;
        submitInfo.signalSemaphoreCount = arraysize32(renderFinishedSemaphores);
        submitInfo.pWaitDstStageMask = waitStages;
        vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, sync.inFlightFences[currentFrame]);

        VkPresentInfoKHR presentInfo;
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pResults = nullptr;
        presentInfo.pWaitSemaphores = renderFinishedSemaphores;
        presentInfo.waitSemaphoreCount = arraysize32(renderFinishedSemaphores);
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &imageIndex;
        const auto result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

        if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
            windowResize();
            return;
        }
    }

    void VulkanInstance::setupSwapchain()
    {
        VkSwapchainKHR oldSwapchain = swapchain;

        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.gpu, surface, &capabilities);

        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.gpu, surface, &presentModeCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device.gpu, surface, &presentModeCount, presentModes.data());

        if (!vSync)
        {
            for (auto mode : presentModes)
            {
                if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    swapchainPresentMode = mode;
                    break;
                }
                if(mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                    swapchainPresentMode = mode;
            }
        }

        auto desiredImageCount = capabilities.minImageCount + 1;
        if((capabilities.maxImageCount > 0) && (desiredImageCount > capabilities.maxImageCount))
            desiredImageCount = capabilities.maxImageCount;

        auto info = Inits::swapchainCreateInfo(VK_NULL_HANDLE);
        info.minImageCount = desiredImageCount;
        info.surface = surface;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;

        if(capabilities.currentExtent.width == UINT32_MAX)
            info.imageExtent = extent;
        else
            info.imageExtent = capabilities.currentExtent;

        // We prefer a non-rotated transform
        if (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
            info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        else
            info.preTransform = capabilities.currentTransform;

        info.preTransform = capabilities.currentTransform;
        info.presentMode = presentMode;

        const uint32_t queueFamilyIndices[] = {
                device.indices.graphics,
                device.indices.present
        };

        if(device.indices.graphics != device.indices.present)
        {
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount = 0;
            info.pQueueFamilyIndices = nullptr;
        }

        vkCreateSwapchainKHR(device, &info, nullptr, &swapchain);

        // If an existing swap chain is re-created, destroy the old swap chain
        // This also cleans up all the presentable images
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            for (auto imageView : swapchainViews)
                vkDestroyImageView(device, imageView, nullptr);

            vkDestroySwapchainKHR(device, oldSwapchain, VK_NULL_HANDLE);
        }

        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
        swapchainViews.resize(imageCount);

        for (size_t i = 0; i < swapchainViews.size(); i++)
        {
            auto imageViewInfo = Inits::imageViewCreateInfo();
            imageViewInfo.format = surfaceFormat.format;
            imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imageViewInfo.image = swapchainImages[i];
            vkCreateImageView(device, &imageViewInfo, nullptr, &swapchainViews[i]);
        }
    }

    void VulkanInstance::setupMsaa()
    {
        auto imageInfo = Inits::imageCreateInfo();
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.extent = {extent.width, extent.height, 1};
        imageInfo.samples = sampleCount;
        imageInfo.format = surfaceFormat.format;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        vkCreateImage(device, &imageInfo, nullptr, &msaa.image);

        VkMemoryRequirements memReqs{};
        vkGetImageMemoryRequirements(device, msaa.image, &memReqs);

        auto allocInfo = device.getMemoryAllocInfo(memReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkAllocateMemory(device, &allocInfo, nullptr, &msaa.memory);
        vkBindImageMemory(device, msaa.image, msaa.memory, 0);

        auto viewInfo = Inits::imageViewCreateInfo();
        viewInfo.image = msaa.image;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkCreateImageView(device, &viewInfo, nullptr, &msaa.view);
    }

    void VulkanInstance::setupDepth()
    {
        auto imageInfo = Inits::imageCreateInfo();
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.extent = {extent.width, extent.height, 1};
        imageInfo.samples = sampleCount;
        imageInfo.format = Tools::DepthFormat(device.gpu);
        imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        vkCreateImage(device, &imageInfo, nullptr, &depth.image);

        VkMemoryRequirements memReqs{};
        vkGetImageMemoryRequirements(device, depth.image, &memReqs);

        auto allocInfo = device.getMemoryAllocInfo(memReqs, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vkAllocateMemory(device, &allocInfo, nullptr, &depth.memory);
        vkBindImageMemory(device, depth.image, depth.memory, 0);

        auto viewInfo = Inits::imageViewCreateInfo();
        viewInfo.image = depth.image;
        viewInfo.format = imageInfo.format;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        vkCreateImageView(device, &viewInfo, nullptr, &depth.view);
    }

    void VulkanInstance::setupRenderPass()
    {
        auto colourAttachment = Inits::attachmentDescription(msaa.format);
        colourAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        colourAttachment.samples = sampleCount;

        auto depthAttachment = Inits::attachmentDescription(depth.format);
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depthAttachment.samples = sampleCount;

        auto colourResolve = Inits::attachmentDescription(msaa.format);
        colourResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colourResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colourResolve.samples = VK_SAMPLE_COUNT_1_BIT;

        const VkAttachmentDescription attachments[] = { colourAttachment, depthAttachment, colourResolve };

        auto colourAttachmentRef = Inits::attachmentReference(0);
        colourAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        auto depthAttachmentRef = Inits::attachmentReference(1);
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        auto colourResolveRef = Inits::attachmentReference(2);
        colourResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colourAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;
        subpass.pResolveAttachments = &colourResolveRef;

        VkSubpassDependency dependencies[2];
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = arraysize32(attachments);
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = arraysize32(dependencies);
        renderPassInfo.pDependencies = dependencies;
        vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
    }

    void VulkanInstance::setupFramebuffers()
    {
        VkImageView attachments[3] = { msaa.view, depth.view };

        auto framebufferInfo = Inits::framebufferCreateInfo();
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = arraysize32(attachments);
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;

        for (size_t i = 0; i < framebuffers.size(); i++)
        {
            attachments[2] = swapchainViews[i];
            vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]);
        }
    }

    void VulkanInstance::setupSyncPrimitives()
    {
        const auto semaphoreInfo = Inits::semaphoreCreateInfo();
        const auto fenceInfo = Inits::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);

        for (size_t i = 0; i < MAX_IMAGES_IN_FLIGHT; i++)
        {
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &sync.imageAvailableSPs[i]);
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &sync.renderFinishedSPs[i]);
            vkCreateFence(device, &fenceInfo, nullptr, &sync.inFlightFences[i]);
        }
    }

    void VulkanInstance::windowResize()
    {
        vkDeviceWaitIdle(device);

        setupSwapchain();

        for (auto framebuffer : framebuffers)
            vkDestroyFramebuffer(device, framebuffer, nullptr);

        for (auto command : commandBuffers)
            vkResetCommandBuffer(command, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);

        setupFramebuffers();
        setupMsaa();
        setupDepth();
        setupRenderPass();
    }
} // vks