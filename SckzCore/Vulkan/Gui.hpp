#include "../Include.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"

namespace sckz
{
    class Gui
    {
    private:
        struct VertexUniformBufferObject
        {
            alignas(16) glm::mat3 model;
        };

        struct FragmentUniformBufferObject
        {
        };

    private:
        VkDevice *                   device;
        VkPhysicalDevice *           physicalDevice;
        VkCommandPool *              commandPool;
        DescriptorPool *             descriptorPool;
        GraphicsPipeline *           pipeline;
        std::vector<VkFramebuffer> * framebuffers;
        VkExtent2D                   swapChainExtent;
        VkQueue *                    queue;
        VkRenderPass *               renderPass;
        Memory *                     memory;

        VkDescriptorSet                 descriptorSet;
        std::vector<VkCommandBuffer>    commandBuffers;
        Buffer                          hostLocalBuffer;
        std::array<Buffer::SubBlock, 2> uniformBuffer;

        const char * textureFileName;
        Image        texture;

        glm::vec2    location;
        glm::vec2    scale;
        glm::float32 rotation;

        bool hasCommandBuffer;

    public:
        void CreateGUI(const char *                 textureFileName,
                       VkCommandPool &              commandPool,
                       VkRenderPass &               renderPass,
                       VkDevice &                   device,
                       VkPhysicalDevice &           physicalDevice,
                       std::vector<VkFramebuffer> & framebuffer,
                       GraphicsPipeline &           pipeline,
                       DescriptorPool &             descriptorPool,
                       VkExtent2D                   swapChainExtent,
                       Memory &                     memory,
                       VkQueue &                    queue);

        void DestroyGUI();

    private:
        void CreateUniformBuffers();
        void CreateCommandBuffer();

    public:
        void SetLocation(float x, float y);
        void SetRotation(float x);
        void SetScale(float x, float y);

        glm::vec2 GetLocation();
        float     GetRotation();
        glm::vec2 GetScale();

        void RebuildSwapResources();

        VkCommandBuffer GetCommandBuffer(uint32_t index);
    };
} // namespace sckz