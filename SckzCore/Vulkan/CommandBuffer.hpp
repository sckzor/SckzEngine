#pragma once
#include "../Include.hpp"

namespace sckz
{
    class CommandBuffer
    {
    private:
        VkCommandBuffer commandBuffer;
        VkDevice *      device;
        VkCommandPool * pool;
        VkQueue *       queue;
        VkFence         inFlightFence;

    public:
        void AllocateSingleUseCommandBuffer(VkDevice & device, VkCommandPool & pool, VkQueue & queue);

        void BeginSingleUseCommandBuffer();

        void EndSingleUseCommandBuffer();

        void FreeSingleUseCommandBuffer();

    private:
        void CreateSyncObjects();

    public:
        VkCommandBuffer & GetCommandBuffer();
    };
} // namespace sckz