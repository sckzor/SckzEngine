#pragma once
#include "../Include.hpp"
#include "DescriptorPool.hpp"
#include "Fbo.hpp"
#include "Image.hpp"
#include "Vertex.hpp"

namespace sckz
{
    class GraphicsPipeline
    {
    public:
        enum class PipelineType
        {
            FBO_PIPELINE,
            GUI_PIPELINE,
            MODEL_PIPELINE,
            PARTICLE_PIPELINE,
            COMBINE_PIPELINE
        };

    private:
        VkDescriptorSetLayout descriptorSetLayout;
        VkPipelineLayout      pipelineLayout;
        VkPipeline            pipeline;

        VkDevice *   device;
        Fbo *        fbo;
        const char * vertexFile;
        const char * fragmentFile;
        PipelineType type;

        uint32_t vertexUboCount   = -1;
        uint32_t samplerCount     = -1;
        uint32_t fragmentUboCount = -1;

    private:
        static std::vector<char> ReadFile(const std::string & filename);
        VkShaderModule           CreateShaderModule(const std::vector<char> & code);

    private:
        void CreateDescriptorSetLayout();
        void CreateGraphicsPipeline();

    public:
        void CreatePipeline(VkDevice &   device,
                            Fbo &        fbo,
                            const char * vertexFile,
                            const char * fragmentFile,
                            PipelineType type);

        void CreatePipeline(VkDevice & device, Fbo & fbo);
        void DestroyPipeline();

    public:
        VkDescriptorSetLayout & GetDescriptorSetLayout();
        VkPipeline &            GetPipeline();
        VkPipelineLayout &      GetPieplineLayout();
        Fbo &                   GetFbo();

        void BindShaderData(Buffer::SubBlock  vUboInfo[],
                            Image             samplerInfo[],
                            Buffer::SubBlock  fUboInfo[],
                            DescriptorPool &  pool,
                            VkDescriptorSet * descriptorSet);

    public:
        bool operator==(GraphicsPipeline & otherObject);
    };
} // namespace sckz