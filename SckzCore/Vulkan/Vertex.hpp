#pragma once
#include "../../Include.hpp"

namespace sckz
{
    struct Vertex
    {
        glm::vec3  pos;
        glm::vec3  normal;
        glm::vec2  texCoord;
        glm::ivec4 boneIds;
        glm::vec4  weights;

        static VkVertexInputBindingDescription GetBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription {};

            bindingDescription.binding   = 0;
            bindingDescription.stride    = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions {};

            attributeDescriptions[0].binding  = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset   = offsetof(Vertex, pos);

            attributeDescriptions[1].binding  = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset   = offsetof(Vertex, normal);

            attributeDescriptions[2].binding  = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format   = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset   = offsetof(Vertex, texCoord);

            attributeDescriptions[3].binding  = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format   = VK_FORMAT_R32G32B32A32_SINT;
            attributeDescriptions[3].offset   = offsetof(Vertex, boneIds);

            attributeDescriptions[4].binding  = 0;
            attributeDescriptions[4].location = 4;
            attributeDescriptions[4].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            attributeDescriptions[4].offset   = offsetof(Vertex, weights);

            return attributeDescriptions;
        }

        bool operator==(const Vertex & other) const
        {
            return pos == other.pos && normal == other.normal && texCoord == other.texCoord;
        }
    };
} // namespace sckz

namespace std
{
    template <>
    struct hash<sckz::Vertex>
    {
        size_t operator()(sckz::Vertex const & vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1)
                 ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
} // namespace std