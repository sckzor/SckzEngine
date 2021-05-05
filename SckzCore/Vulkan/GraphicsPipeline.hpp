#pragma once
#include "../Include.hpp"
#include "Vertex.hpp"

namespace sckz
{
    class GraphicsPipeline
    {
    private:
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout      pipelineLayout;
        VkPipeline            pipeline;

        VkDevice *            device;
        VkExtent2D            extent;
        VkRenderPass *        renderPass;
        VkSampleCountFlagBits msaaSamples;
        const char *          vertexFile;
        const char *          fragmentFile;
        bool                  isFBO;

    private:
        static std::vector<char> ReadFile(const std::string & filename);
        VkShaderModule           CreateShaderModule(const std::vector<char> & code);

    public:
        void CreateDescriptorSetLayout();
        void CreateGraphicsPipeline();

    public:
        void CreatePipeline(VkDevice &            device,
                            VkExtent2D            extent,
                            VkRenderPass &        renderPass,
                            VkSampleCountFlagBits msaaSamples,
                            const char *          vertexFile,
                            const char *          fragmentFile,
                            bool                  isFBO);

        void CreatePipeline(VkDevice &            device,
                            VkExtent2D            extent,
                            VkRenderPass &        renderPass,
                            VkSampleCountFlagBits msaaSamples,
                            bool                  isFBO);
        void DestroyPipeline();

    public:
        VkDescriptorSetLayout & GetDescriptorSetLayout();
        VkPipeline &            GetPipeline();
        VkPipelineLayout &      GetPieplineLayout();

    public:
        bool operator==(GraphicsPipeline & otherObject);
    };
} // namespace sckz