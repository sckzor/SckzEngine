#include "CommandBuffer.hpp"

namespace sckz
{
    void CommandBuffer::AllocateSingleUseCommandBuffer(VkDevice & device, VkCommandPool & pool, VkQueue & queue)
    {
        this->device = &device;
        this->pool   = &pool;
        this->queue  = &queue;
        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = *this->pool;
        allocInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(*this->device, &allocInfo, &this->commandBuffer);
    }

    void CommandBuffer::BeginSingleUseCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(this->commandBuffer, &beginInfo);
    }

    void CommandBuffer::EndSingleUseCommandBuffer()
    {
        vkEndCommandBuffer(this->commandBuffer);

        VkSubmitInfo submitInfo {};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &this->commandBuffer;

        vkQueueSubmit(*this->queue, 1, &submitInfo, VK_NULL_HANDLE);
    }

    void CommandBuffer::FreeSingleUseCommandBuffer()
    {
        vkFreeCommandBuffers(*this->device, *this->pool, 1, &this->commandBuffer);
    }

    VkCommandBuffer & CommandBuffer::GetCommandBuffer() { return commandBuffer; }
} // namespace sckz