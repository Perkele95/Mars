//
// Created by arlev on 21.12.2022.
//

#pragma once

#include "VulkanInitialisers.hpp"

namespace vks::Tools
{
#define TOOLS_API inline

    TOOLS_API VkSampleCountFlagBits SampleCount(VkPhysicalDevice gpu)
    {
        VkPhysicalDeviceProperties props{};
        vkGetPhysicalDeviceProperties(gpu, &props);

        const auto samples = props.limits.framebufferDepthSampleCounts &
                             props.limits.framebufferColorSampleCounts;

        auto sampleCount = VK_SAMPLE_COUNT_1_BIT;
        for (uint32_t bit = VK_SAMPLE_COUNT_64_BIT; bit != VK_SAMPLE_COUNT_1_BIT; bit >>= 1)
        {
            if (samples & bit)
            {
                sampleCount = VkSampleCountFlagBits(bit);
                break;
            }
        }
        return sampleCount;
    }

    TOOLS_API VkFormat DepthFormat(VkPhysicalDevice gpu)
    {
        constexpr VkFormat formats[] = {
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D32_SFLOAT_S8_UINT
        };
        auto depthFormat = formats[0];

        for (auto format : formats)
        {
            VkFormatProperties props{};
            vkGetPhysicalDeviceFormatProperties(gpu, format, &props);
            if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                depthFormat = format;
                break;
            }
        }
        return depthFormat;
    }

    TOOLS_API bool LinearFilterSupport(VkPhysicalDevice gpu, VkFormat format, VkImageTiling tiling)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

        switch (tiling)
        {
            case VK_IMAGE_TILING_LINEAR:
                return props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
            case VK_IMAGE_TILING_OPTIMAL:
                return props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
            default: return false;
        }
    }

    TOOLS_API bool BlitCmdSupport(VkPhysicalDevice gpu, VkFormat format, VkImageTiling tiling)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

        switch (tiling)
        {
            case VK_IMAGE_TILING_OPTIMAL:
                return props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT &&
                       props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT;
            case VK_IMAGE_TILING_LINEAR:
                return props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT &&
                       props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT;
            default: return false;
        }
    }

    TOOLS_API bool BlitCmdSupportOptimal(VkPhysicalDevice gpu, VkFormat format)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

        return props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT &&
               props.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT;
    }

    TOOLS_API bool BlitCmdSupportLinear(VkPhysicalDevice gpu, VkFormat format)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

        return props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT &&
               props.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT;
    }

    template<int N>
    TOOLS_API uint32_t DescriptorPoolMaxSets(const VkDescriptorPoolSize(&poolSizes)[N])
    {
        uint32_t maxSets = 0;
        for (uint32_t i = 0; i < N; i++)
            maxSets += poolSizes[i].descriptorCount;

        return maxSets;
    }

    TOOLS_API void InsertMemoryBarrier(VkCommandBuffer cmd,
                                       VkImageLayout oldLayout,
                                       VkImageLayout newLayout,
                                       VkAccessFlags srcAccessMask,
                                       VkAccessFlags dstAccessMask,
                                       VkPipelineStageFlags srcStageMask,
                                       VkPipelineStageFlags dstStageMask,
                                       const VkImageSubresourceRange &subresourceRange,
                                       VkImage image)
    {
        auto barrier = Inits::imageMemoryBarrier(image, oldLayout, newLayout);
        barrier.srcAccessMask = srcAccessMask;
        barrier.dstAccessMask = dstAccessMask;
        barrier.subresourceRange = subresourceRange;
        vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask,
                             0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    TOOLS_API void SetImageLayout(VkCommandBuffer cmd,
                                  VkImageLayout oldLayout,
                                  VkImageLayout newLayout,
                                  VkPipelineStageFlags srcStageMask,
                                  VkPipelineStageFlags dstStageMask,
                                  VkImage image,
                                  VkImageSubresourceRange subresourceRange = Inits::imageSubresourceRange())
    {
        auto imageBarrier = Inits::imageMemoryBarrier(image, oldLayout, newLayout);
        imageBarrier.subresourceRange = subresourceRange;

        switch (oldLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                imageBarrier.srcAccessMask = VK_ACCESS_NONE;
                break;
            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                imageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                imageBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default: break;
        }

        switch (newLayout)
        {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                break;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                imageBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                imageBarrier.dstAccessMask = imageBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                if (imageBarrier.srcAccessMask == VK_ACCESS_NONE)
                    imageBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                break;
            default: break;
        }

        vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0, 0, nullptr,
                             0, nullptr, 1, &imageBarrier);
    }

    TOOLS_API void PushConstants(VkCommandBuffer command,
                                 VkPipelineLayout layout,
                                 VkPushConstantRange range,
                                 const void *pValues)
    {
        vkCmdPushConstants(command, layout, range.stageFlags, range.offset, range.size, pValues);
    }

    TOOLS_API VkResult LoadShader(VkDevice device, const void *data, size_t dataSize, VkShaderModule *pModule)
    {
        auto loadInfo = Inits::shaderModuleCreateInfo(data, dataSize);
        return vkCreateShaderModule(device, &loadInfo, nullptr, pModule);
    }

#undef TOOLS_API
}