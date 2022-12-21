//
// Created by arlev on 21.12.2022.
//

#pragma once

#include "../../Core/Base.hpp"
#include <vulkan/vulkan.h>

namespace vks::Inits
{
#define INIT_API constexpr auto

    INIT_API applicationInfo(const char *appName)
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName;
        appInfo.pEngineName = appName;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        return appInfo;
    }

    INIT_API instanceCreateInfo()
    {
        VkInstanceCreateInfo iInfo{};
        iInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        return iInfo;
    }

    INIT_API commandPoolCreateInfo(uint32_t queueFamilyIndex)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndex;
        return poolInfo;
    }

    INIT_API swapchainCreateInfo(VkSwapchainKHR oldSwapchain)
    {
        VkSwapchainCreateInfoKHR info{};
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        info.oldSwapchain = oldSwapchain;
        info.imageArrayLayers = 1;
        info.clipped = VK_TRUE;
        return info;
    }

    INIT_API framebufferCreateInfo()
    {
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.layers = 1;
        return info;
    }

    INIT_API descriptorPoolCreateInfo(const VkDescriptorPoolSize *pPoolsizes,
                                      size_t poolSizeCount, uint32_t maxSets)
    {
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pPoolSizes = pPoolsizes;
        info.poolSizeCount = uint32_t(poolSizeCount);
        info.maxSets = maxSets;
        return info;
    }

    template<uint32_t N>
    INIT_API descriptorPoolCreateInfo(const VkDescriptorPoolSize(&poolsizes)[N], uint32_t maxSets)
    {
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.pPoolSizes = poolsizes;
        info.poolSizeCount = N;
        info.maxSets = maxSets;
        return info;
    }

    INIT_API descriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding *pBindings,
                                           size_t bindingCount)
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pBindings = pBindings;
        layoutInfo.bindingCount = uint32_t(bindingCount);
        return layoutInfo;
    }

    template<uint32_t N>
    INIT_API descriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding(&bindings)[N])
    {
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.pBindings = bindings;
        layoutInfo.bindingCount = N;
        return layoutInfo;
    }

    INIT_API descriptorSetAllocateInfo(VkDescriptorPool pool, const VkDescriptorSetLayout *pSetLayouts,
                                       size_t descriptorSetCount)
    {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = uint32_t(descriptorSetCount);
        info.pSetLayouts = pSetLayouts;
        return info;
    }

    template<uint32_t N>
    INIT_API descriptorSetAllocateInfo(VkDescriptorPool pool, const VkDescriptorSetLayout(&setLayouts)[N])
    {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = N;
        info.pSetLayouts = setLayouts;
        return info;
    }

    INIT_API descriptorPoolSize(VkDescriptorType type, size_t count)
    {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = type;
        poolSize.descriptorCount = uint32_t(count);
        return poolSize;
    }

    INIT_API descriptorSetLayoutBinding(uint32_t binding,
                                        VkDescriptorType type,
                                        VkShaderStageFlags stageFlags)
    {
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = type;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = stageFlags;
        layoutBinding.pImmutableSamplers = nullptr;
        return layoutBinding;
    }

    INIT_API writeDescriptorSet(uint32_t binding,
                                VkDescriptorType type,
                                VkDescriptorSet dstSet,
                                const VkDescriptorBufferInfo *pBufferInfo)
    {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.dstSet = dstSet;
        write.pBufferInfo = pBufferInfo;
        return write;
    }

    INIT_API writeDescriptorSet(uint32_t binding,
                                VkDescriptorType type,
                                VkDescriptorSet dstSet,
                                const VkDescriptorImageInfo *pImageInfo)
    {
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.dstSet = dstSet;
        write.pImageInfo = pImageInfo;
        return write;
    }

    INIT_API attachmentDescription(VkFormat format)
    {
        VkAttachmentDescription attachmentDescription{};
        attachmentDescription.format = format;
        attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        return attachmentDescription;
    }

    INIT_API attachmentReference(uint32_t attachment)
    {
        VkAttachmentReference ref{};
        ref.attachment = attachment;
        ref.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        return ref;
    }

    INIT_API shaderModuleCreateInfo(const void *source, size_t size)
    {
        VkShaderModuleCreateInfo shaderModuleInfo{};
        shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.codeSize = size;
        shaderModuleInfo.pCode = static_cast<const uint32_t *>(source);
        return shaderModuleInfo;
    }

    INIT_API bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usageFlags)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usageFlags;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        return bufferInfo;
    }

    INIT_API commandBufferAllocateInfo(VkCommandPool commandPool, size_t count)
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = static_cast<uint32_t>(count);
        return allocInfo;
    }

    INIT_API commandBufferBeginInfo(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;
        return beginInfo;
    }

    INIT_API renderPassBeginInfo(VkRenderPass renderPass, VkExtent2D extent)
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = extent;
        return renderPassInfo;
    }

    INIT_API submitInfo(const VkCommandBuffer *pCommandBuffers, size_t commandBufferCount)
    {
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = static_cast<uint32_t>(commandBufferCount);
        submitInfo.pCommandBuffers = pCommandBuffers;
        return submitInfo;
    }

    INIT_API imageCreateInfo()
    {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        return imageCreateInfo;
    }

    INIT_API imageViewCreateInfo()
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.pNext = nullptr;
        viewInfo.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
        };
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        return viewInfo;
    }

    INIT_API samplerCreateInfo(float maxLod = 1.0f)
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.maxLod = maxLod;
        return samplerInfo;
    }

    INIT_API bufferImageCopy(VkExtent2D extent)
    {
        VkBufferImageCopy copyRegion{};
        copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyRegion.imageSubresource.layerCount = 1;
        copyRegion.imageOffset = { 0, 0, 0 };
        copyRegion.imageExtent = { extent.width, extent.height, 1 };
        return copyRegion;
    }

    INIT_API bufferCopy(VkDeviceSize size)
    {
        VkBufferCopy bufferCopy{};
        bufferCopy.size = size;
        return bufferCopy;
    }

    INIT_API imageSubresourceRange()
    {
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.levelCount = 1;
        subresourceRange.layerCount = 1;
        return subresourceRange;
    }

    INIT_API imageMemoryBarrier(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcAccessMask = VK_ACCESS_NONE;
        barrier.dstAccessMask = VK_ACCESS_NONE;
        return barrier;
    }

    INIT_API descriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = offset;
        bufferInfo.range = range;
        return bufferInfo;
    }

    INIT_API semaphoreCreateInfo()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        return semaphoreInfo;
    }

    INIT_API fenceCreateInfo(VkFenceCreateFlags flags)
    {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = flags;// VK_FENCE_CREATE_SIGNALED_BIT;
        return fenceInfo;
    }

    INIT_API shaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module)
    {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = stage;
        stageInfo.module = module;
        stageInfo.pName = "main";
        return stageInfo;
    }

    INIT_API vertexBindingDescription(uint32_t stride)
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    INIT_API inputAssemblyInfo()
    {
        VkPipelineInputAssemblyStateCreateInfo result{};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        result.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        result.primitiveRestartEnable = VK_FALSE;
        return result;
    }

    INIT_API pipelineViewportStateCreateInfo(uint32_t viewportCount, uint32_t scissorCount)
    {
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = viewportCount;
        viewportState.scissorCount = scissorCount;
        return viewportState;
    }

    INIT_API viewportInfo(VkExtent2D extent)
    {
        VkViewport result{};
        result.width = static_cast<float>(extent.width);
        result.height = static_cast<float>(extent.height);
        result.minDepth = 0.0f;
        result.maxDepth = 1.0f;
        return result;
    }

    INIT_API scissorInfo(VkExtent2D extent)
    {
        VkRect2D result{};
        result.offset = { 0, 0 };
        result.extent = extent;
        return result;
    }

    INIT_API rasterizationStateInfo(VkFrontFace frontFace)
    {
        VkPipelineRasterizationStateCreateInfo result = {};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        result.depthClampEnable = VK_FALSE;
        result.rasterizerDiscardEnable = VK_FALSE;
        result.polygonMode = VK_POLYGON_MODE_FILL;
        result.lineWidth = 1.0f;
        result.cullMode = VK_CULL_MODE_BACK_BIT;
        result.frontFace = frontFace;
        result.depthBiasEnable = VK_FALSE;
        return result;
    }

    INIT_API depthStencilStateInfo()
    {
        VkPipelineDepthStencilStateCreateInfo result = {};
        result.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        result.depthTestEnable = VK_TRUE;
        result.depthWriteEnable = VK_TRUE;
        result.depthCompareOp = VK_COMPARE_OP_LESS;
        result.depthBoundsTestEnable = VK_FALSE;
        result.stencilTestEnable = VK_FALSE;
        return result;
    }

    INIT_API colourWriteMask()
    {
        VkColorComponentFlags writeMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
                                          | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        return writeMask;
    }

    template<int N>
    INIT_API pipelineColorBlendStateCreateInfo(const VkPipelineColorBlendAttachmentState(&attachments)[N])
    {
        VkPipelineColorBlendStateCreateInfo colourblend{};
        colourblend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colourblend.logicOpEnable = VK_FALSE;
        colourblend.logicOp = VK_LOGIC_OP_COPY;
        colourblend.blendConstants[0] = 0.0f;
        colourblend.blendConstants[1] = 0.0f;
        colourblend.blendConstants[2] = 0.0f;
        colourblend.blendConstants[3] = 0.0f;
        colourblend.pAttachments = attachments;
        colourblend.attachmentCount = arraysize32(attachments);
        return colourblend;
    }

    template<int N>
    INIT_API pipelineDynamicStateCreateInfo(const VkDynamicState(&states)[N])
    {
        VkPipelineDynamicStateCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        info.pDynamicStates = states;
        info.dynamicStateCount = arraysize32(states);
        return info;
    }

    INIT_API pushConstantRange(VkShaderStageFlags stage, uint32_t size, uint32_t offset = 0)
    {
        VkPushConstantRange push;
        push.offset = offset;
        push.size = size;
        push.stageFlags = stage;
        return push;
    }

    INIT_API pipelineMultisampleStateCreateInfo(VkSampleCountFlagBits rasterizationSamples)
    {
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = rasterizationSamples;
        return multisampling;
    }

    INIT_API pipelineColorBlendAttachmentState()
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = colourWriteMask();
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        return colorBlendAttachment;
    }
#undef INIT_API
}