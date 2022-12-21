//
// Created by arlev on 21.12.2022.
//

#include "VulkanDevice.hpp"

namespace vks
{
    void VulkanDevice::initialise(VkInstance instance, VkSurfaceKHR surface)
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        auto physicalDevices = std::vector<VkPhysicalDevice>(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

        gpu = physicalDevices[0];

        for (auto candidate : physicalDevices)
        {
            const auto extensionsSupported = checkDeviceExtensions(candidate);
            const auto adequateCapabilities = checkSurfaceFormatSupport(candidate, surface);
            const auto adequateIndices = checkQueueIndices(candidate, surface);

            if (extensionsSupported && adequateCapabilities && adequateIndices)
            {
                gpu = candidate;
                break;
            }
        }

        vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
        vkGetPhysicalDeviceMemoryProperties(gpu, &memProps);

        const uint32_t queueFamilies[] = { indices.graphics, indices.present };
        const uint32_t queueCount = (indices.graphics != indices.present) ? 2 : 1;

        VkDeviceQueueCreateInfo queueCreateInfos[2] = {};

        float queuePriority = 1.0f;
        for (size_t i = 0; i < queueCount; i++)
        {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = queueFamilies[i];
            queueCreateInfos[i].queueCount = queueCount;
            queueCreateInfos[i].pQueuePriorities = &queuePriority;
            queueCreateInfos[i].flags = 0;
            queueCreateInfos[i].pNext = nullptr;
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.sampleRateShading = VK_TRUE;

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = queueCount;
        createInfo.pQueueCreateInfos = queueCreateInfos;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = arraysize32(DeviceExtensions);
        createInfo.ppEnabledExtensionNames = DeviceExtensions;
#ifdef ENABLE_VALIDATION
        createInfo.enabledLayerCount = arraysize32(ValidationLayers);
        createInfo.ppEnabledLayerNames = ValidationLayers;
#else
        createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
#endif
        vkCreateDevice(gpu, &createInfo, nullptr, &device);

        auto poolInfo = Inits::commandPoolCreateInfo(indices.graphics);
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);

        vkGetDeviceQueue(device, indices.graphics, 0, &graphicsQueue);
        vkGetDeviceQueue(device, indices.present, 0, &presentQueue);

        VkPipelineCacheCreateInfo pipelineCacheInfo{};
        pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        pipelineCacheInfo.pInitialData = VK_NULL_HANDLE;
        pipelineCacheInfo.initialDataSize = 0;
        vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &pipelineCache);
    }

    void VulkanDevice::shutdown()
    {
        vkDestroyPipelineCache(device, pipelineCache, nullptr);
        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyDevice(device, nullptr);
    }

    VkMemoryAllocateInfo
    VulkanDevice::getMemoryAllocInfo(VkMemoryRequirements memReqs, VkMemoryPropertyFlags flags) const
    {
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReqs.size;
        allocInfo.memoryTypeIndex = 0;

        for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
        {
            if ((memReqs.memoryTypeBits & BIT(i)) && (memProps.memoryTypes[i].propertyFlags & flags))
            {
                allocInfo.memoryTypeIndex = i;
                break;
            }
        }

        return allocInfo;
    }

    VkCommandBuffer VulkanDevice::createCommandBuffer(VkCommandBufferLevel level, bool begin) const
    {
        VkCommandBuffer command = VK_NULL_HANDLE;

        auto allocateInfo = Inits::commandBufferAllocateInfo(commandPool, 1);
        allocateInfo.level = level;
        vkAllocateCommandBuffers(device, &allocateInfo, &command);

        if (begin)
        {
            const auto beginInfo = Inits::commandBufferBeginInfo(0);
            vkBeginCommandBuffer(command, &beginInfo);
        }

        return command;
    }

    void VulkanDevice::flushCommandBuffer(VkCommandBuffer command, VkQueue queue, bool free) const
    {
        vkEndCommandBuffer(command);

        auto fenceInfo = Inits::fenceCreateInfo(0);
        VkFence fence = VK_NULL_HANDLE;
        vkCreateFence(device, &fenceInfo, nullptr, &fence);

        auto submitInfo = Inits::submitInfo(&command, 1);
        vkQueueSubmit(queue, 1, &submitInfo, fence);
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(device, fence, nullptr);

        if (free)
            vkFreeCommandBuffers(device, commandPool, 1, &command);
    }

    bool VulkanDevice::checkDeviceExtensions(VkPhysicalDevice pd)
    {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(pd, nullptr, &extensionCount, nullptr);
        auto extensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(pd, nullptr, &extensionCount, extensions.data());

        for (size_t i = 0; i < arraysize(DeviceExtensions); i++)
        {
            for (auto& extension : extensions)
            {
                if (std::strcmp(DeviceExtensions[i], extension.extensionName) == 0)
                    return true;
            }
        }

        return false;
    }

    bool VulkanDevice::checkSurfaceFormatSupport(VkPhysicalDevice pd, VkSurfaceKHR surface)
    {
        uint32_t formatCount = 0, presentModeCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(pd, surface, &formatCount, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(pd, surface, &presentModeCount, nullptr);
        return (formatCount > 0) && (presentModeCount > 0);
    }

    bool VulkanDevice::checkQueueIndices(VkPhysicalDevice pd, VkSurfaceKHR surface)
    {
        uint32_t propCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &propCount, nullptr);
        auto properties = std::vector<VkQueueFamilyProperties>(propCount);
        vkGetPhysicalDeviceQueueFamilyProperties(pd, &propCount, properties.data());

        VkBool32 graphics = VK_FALSE, present = VK_FALSE;
        indices = {};

        for (uint32_t i = 0; i < properties.size(); i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &present);

            if (present)
                indices.present = i;

            if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphics = i;
                graphics = VK_TRUE;
            }

            if (present && graphics)
                break;
        }

        return present && graphics;
    }
} // vks