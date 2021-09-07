#pragma once
#include "../../Include.hpp"
#include "BoneInfo.hpp"

#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace sckz
{
    class Bone
    {
    private:
        struct KeyPosition
        {
            glm::vec3 position;
            float     timeStamp;
        };

        struct KeyRotation
        {
            glm::quat orientation;
            float     timeStamp;
        };

        struct KeyScale
        {
            glm::vec3 scale;
            float     timeStamp;
        };

    private:
        std::vector<KeyPosition> positions;
        std::vector<KeyRotation> rotations;
        std::vector<KeyScale>    scales;
        uint32_t                 numPositions;
        uint32_t                 numRotations;
        uint32_t                 numScales;

        glm::mat4   localTransform;
        std::string name;
        uint32_t    id;

    public:
        void CreateBone(std::string name, uint32_t ID, const aiNodeAnim * channel);

        /* Interpolates b/w positions,rotations & scaling keys based on the curren time of the
        animation and prepares the local transformation matrix by combining all keys tranformations */
        void Update(float animationTime);

        glm::mat4   GetLocalTransform();
        std::string GetBoneName() const;
        uint32_t    GetBoneID();

        /* Gets the current index on mKeyPositions to interpolate to based on the current
        animation time */
        uint32_t GetPositionIndex(float animationTime);

        /* Gets the current index on mKeyRotations to interpolate to based on the current
        animation time */
        uint32_t GetRotationIndex(float animationTime);

        /* Gets the current index on mKeyScalings to interpolate to based on the current
        animation time */
        uint32_t GetScaleIndex(float animationTime);

    private:
        /* Gets normalized value for Lerp & Slerp*/
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

        /* figures out which position keys to interpolate b/w and performs the interpolation
        and returns the translation matrix */
        glm::mat4 InterpolatePosition(float animationTime);

        /* figures out which rotations keys to interpolate b/w and performs the interpolation
        and returns the rotation matrix */
        glm::mat4 InterpolateRotation(float animationTime);

        /* figures out which scaling keys to interpolate b/w and performs the interpolation
        and returns the scale matrix */
        glm::mat4 InterpolateScaling(float animationTime);

    private:
    };
} // namespace sckz