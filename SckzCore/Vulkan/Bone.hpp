#pragma once
#include "../../Include.hpp"

#include <assimp/scene.h>

namespace sckz
{
    struct Bone
    {
        const char * name;
        glm::mat4    offsetMatrix;
        glm::mat4    finalTransformation;

        static glm::mat4 Mat4x4FromAssimp(aiMatrix4x4 assimpMat)
        {
            glm::mat4 matrix;

            for (uint32_t i = 0; i < 4; i++)
            {
                for (uint32_t j = 0; j < 4; j++)
                {
                    matrix[i][j] = assimpMat[i][j];
                }
            }

            return matrix;
        }
    };
} // namespace sckz