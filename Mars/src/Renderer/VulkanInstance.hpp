//
// Created by arlev on 21.12.2022.
//

#pragma once

#include "VulkanDevice.hpp"

struct GLFWwindow;

namespace vks
{
    struct FramebufferAttachment
    {
        VkImage             image;
        VkImageView         view;
        VkDeviceMemory      memory;
        VkFormat            format;
    };

    struct SyncObjects
    {
        VkSemaphore         imageAvailableSPs[MAX_IMAGES_IN_FLIGHT];
        VkSemaphore         renderFinishedSPs[MAX_IMAGES_IN_FLIGHT];
        VkFence             inFlightFences[MAX_IMAGES_IN_FLIGHT];
    };

    class VulkanInstance
    {
    protected:
        void initialise(GLFWwindow *window, VkExtent2D screenExtent);
        void shutdown();
        void prepareFrame();
        void submitFrame();

        void setVsync(bool value)
        {
            vSync = value;
            windowResize();
        }

        virtual void buildCommandBuffers() = 0;

        VkCommandBuffer	            commandBuffers[MAX_IMAGES_IN_FLIGHT];

    private:
        void setupSwapchain();
        void setupMsaa();
        void setupDepth();
        void setupRenderPass();
        void setupFramebuffers();
        void setupSyncPrimitives();
        void windowResize();

        VkInstance					instance;
        VkSurfaceKHR				surface;
        VulkanDevice				device;
        VkSwapchainKHR				swapchain;
        VkPresentModeKHR            presentMode;
        VkSurfaceFormatKHR          surfaceFormat;
        VkSampleCountFlagBits       sampleCount;
        VkRenderPass                renderPass;
        VkExtent2D					extent;
        std::vector<VkFramebuffer>	framebuffers;
        std::vector<VkImageView>	swapchainViews;
        std::vector<VkImage>		swapchainImages;

        FramebufferAttachment       msaa, depth;
        SyncObjects                 sync;

        uint32_t					imageCount;
        uint32_t		            currentFrame;
        uint32_t		            imageIndex;
        bool                        vSync;
    };
} // vks
