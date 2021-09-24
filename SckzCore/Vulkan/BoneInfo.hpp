#pragma once
#include "../../Include.hpp"

#include <assimp/scene.h>

namespace sckz
{
    struct BoneInfo
    {
        static const uint32_t MAX_BONES_PER_VERTEX = 4;

        uint32_t  id;
        glm::mat4 offset;

        static glm::mat4 Mat4x4FromAssimp(aiMatrix4x4 assimpMat)
        {
            glm::mat4 matrix;

            for (uint32_t i = 0; i < 4; i++)
            {
                for (uint32_t j = 0; j < 4; j++)
                {
                    matrix[i][j] = assimpMat[j][i];
                }
            }

            return matrix;
        }

        static glm::vec3 Vec3FromAssimp(aiVector3D assimpVec)
        {
            return glm::vec3(assimpVec.x, assimpVec.y, assimpVec.z);
        }

        static glm::quat QuatFromAssimp(const aiQuaternion & pOrientation)
        {
            return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
        }
    };
} // namespace sckz