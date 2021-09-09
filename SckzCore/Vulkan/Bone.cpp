#include "Bone.hpp"

namespace sckz
{
    void Bone::CreateBone(std::string name, uint32_t ID, const aiNodeAnim * channel)
    {
        this->name     = name;
        id             = ID;
        localTransform = glm::mat4(1.0f);

        numPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
        {
            aiVector3D  aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float       timeStamp  = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position  = BoneInfo::Vec3FromAssimp(aiPosition);
            data.timeStamp = timeStamp;
            positions.push_back(data);
        }

        numRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float        timeStamp     = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation  data;
            data.orientation = BoneInfo::QuatFromAssimp(aiOrientation);
            data.timeStamp   = timeStamp;
            rotations.push_back(data);
        }

        numScales = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < numScales; ++keyIndex)
        {
            aiVector3D scale     = channel->mScalingKeys[keyIndex].mValue;
            float      timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale   data;
            data.scale     = BoneInfo::Vec3FromAssimp(scale);
            data.timeStamp = timeStamp;
            scales.push_back(data);
        }
    }

    /* Interpolates b/w positions,rotations & scaling keys based on the curren time of the
    animation and prepares the local transformation matrix by combining all keys tranformations */
    void Bone::Update(float animationTime)
    {
        glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation    = InterpolateRotation(animationTime);
        glm::mat4 scale       = InterpolateScaling(animationTime);
        localTransform        = translation * rotation * scale;
    }

    glm::mat4   Bone::GetLocalTransform() { return localTransform; }
    std::string Bone::GetBoneName() const { return name; }
    uint32_t    Bone::GetBoneID() { return id; }

    /* Gets the current index on mKeyPositions to interpolate to based on the current
    animation time */
    uint32_t Bone::GetPositionIndex(float animationTime)
    {
        for (int index = 0; index < numPositions - 1; ++index)
        {
            if (animationTime < positions[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    /* Gets the current index on mKeyRotations to interpolate to based on the current
    animation time */
    uint32_t Bone::GetRotationIndex(float animationTime)
    {
        for (int index = 0; index < numRotations - 1; ++index)
        {
            if (animationTime < rotations[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }

    /* Gets the current index on mKeyScalings to interpolate to based on the current
    animation time */
    uint32_t Bone::GetScaleIndex(float animationTime)
    {
        for (int index = 0; index < numScales - 1; ++index)
        {
            if (animationTime < scales[index + 1].timeStamp)
                return index;
        }
        assert(0);
    }
    /* Gets normalized value for Lerp & Slerp*/
    float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor  = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff   = nextTimeStamp - lastTimeStamp;
        scaleFactor        = midWayLength / framesDiff;
        return scaleFactor;
    }

    /* figures out which position keys to interpolate b/w and performs the interpolation
    and returns the translation matrix */
    glm::mat4 Bone::InterpolatePosition(float animationTime)
    {
        if (1 == numPositions)
        {
            return glm::translate(glm::mat4(1.0f), positions[0].position);
        }

        int   p0Index     = GetPositionIndex(animationTime);
        int   p1Index     = p0Index + 1;
        float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
        glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    /* figures out which rotations keys to interpolate b/w and performs the interpolation
    and returns the rotation matrix */
    glm::mat4 Bone::InterpolateRotation(float animationTime)
    {
        if (1 == numRotations)
        {
            auto rotation = glm::normalize(rotations[0].orientation);
            return glm::mat4_cast(rotation);
        }

        int   p0Index     = GetRotationIndex(animationTime);
        int   p1Index     = p0Index + 1;
        float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
        glm::quat finalRotation
            = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::mat4_cast(finalRotation);
    }

    /* figures out which scaling keys to interpolate b/w and performs the interpolation
    and returns the scale matrix */
    glm::mat4 Bone::InterpolateScaling(float animationTime)
    {
        if (1 == numScales)
            return glm::scale(glm::mat4(1.0f), scales[0].scale);

        int       p0Index     = GetScaleIndex(animationTime);
        int       p1Index     = p0Index + 1;
        float     scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
        glm::vec3 finalScale  = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
} // namespace sckz