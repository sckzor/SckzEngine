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
        /* WARNING THIS OPPERATION IS EXTREMELY EXPESNIVE! CALL AS LITTLE AS POSSIBLE DURING OPPERATION!!!! */
        void BeginSingleUseCommandBuffer(VkDevice & device, VkCommandPool & pool, VkQueue & queue);

        /* WARNING THIS OPPERATION IS EXTREMELY EXPESNIVE! CALL AS LITTLE AS POSSIBLE DURING OPPERATION!!!! */
        void EndSingleUseCommandBuffer();

    public:
        VkCommandBuffer & GetCommandBuffer();
    };
} // namespace sckz