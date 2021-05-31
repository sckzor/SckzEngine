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
            alignas(16) glm::vec4 location;
            alignas(16) glm::vec4 scale;
            alignas(16) glm::vec4 rotation;
        };

        struct FragmentUniformBufferObject
        {
            alignas(16) glm::vec4 dummy;
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
        glm::vec2    rotationPoint;
        glm::float32 rotation;

        bool hasCommandBuffer;

    public:
        void CreateGUI(const char *                 textureFileName,
                       VkCommandPool &              commandPool,
                       VkRenderPass &               renderPass,
                       VkDevice &                   device,
                       VkPhysicalDevice &           physicalDevice,
                       std::vector<VkFramebuffer> & framebuffers,
                       GraphicsPipeline &           pipeline,
                       DescriptorPool &             descriptorPool,
                       VkExtent2D                   swapChainExtent,
                       Memory &                     memory,
                       VkQueue &                    queue);

        void Update();

        void DestroyGUI();

    private:
        void CreateUniformBuffers();
        void CreateCommandBuffer();

        void DestroySwapResources();

    public:
        void SetLocation(float x, float y);
        void SetScale(float x, float y);
        void SetRotationPoint(float x, float y);
        void SetRotation(float x);

        glm::vec2 GetLocation();
        glm::vec2 GetScale();
        glm::vec2 GetRotationPoint();
        float     GetRotation();

        void RebuildSwapResources(DescriptorPool &             descriptorPool,
                                  VkExtent2D                   swapChainExtent,
                                  VkRenderPass &               renderPass,
                                  std::vector<VkFramebuffer> & framebuffers);

        VkCommandBuffer GetCommandBuffer(uint32_t index);
    };
} // namespace sckz