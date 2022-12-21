//
// Created by arlev on 21.12.2022.
//

#pragma once

#include "Base/VulkanInitialisers.hpp"
#include "Base/VulkanTools.hpp"

namespace vks
{
#ifdef ENABLE_VALIDATION
    constexpr const char *ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };
#endif

    constexpr size_t MAX_IMAGES_IN_FLIGHT = 2;

    class VulkanDevice
    {
        static constexpr const char *DeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    public:
        constexpr operator VkDevice() const
        {
            return device;
        }

        void initialise(VkInstance instance, VkSurfaceKHR surface);
        void shutdown();

        VkMemoryAllocateInfo getMemoryAllocInfo(VkMemoryRequirements memReqs, VkMemoryPropertyFlags flags) const;
        VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin = true) const;
        void flushCommandBuffer(VkCommandBuffer command, VkQueue queue, bool free = true) const;

        void flushCommandBuffer(VkCommandBuffer command, bool free = true) const
        {
            flushCommandBuffer(command, graphicsQueue, free);
        }

        VkDevice                            device;
        VkPhysicalDevice                    gpu;
        VkPhysicalDeviceProperties          gpuProperties;
        VkPhysicalDeviceMemoryProperties    memProps;
        VkQueue                             graphicsQueue;
        VkQueue                             presentQueue;
        VkPipelineCache                     pipelineCache;
        VkCommandPool                       commandPool;

        struct
        {
            uint32_t graphics;
            uint32_t present;
        }indices;

    private:
        bool checkDeviceExtensions(VkPhysicalDevice pd);
        bool checkSurfaceFormatSupport(VkPhysicalDevice pd, VkSurfaceKHR surface);
        bool checkQueueIndices(VkPhysicalDevice pd, VkSurfaceKHR surface);
    };
} // vks