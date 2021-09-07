#include "Animator.hpp"

namespace sckz
{
    void Animator::CreateAnimator(Animation * Animation)
    {
        currentTime            = 0.0;
        this->currentAnimation = Animation;

        finalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            finalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void Animator::UpdateAnimation(float dt)
    {
        deltaTime = dt;
        if (currentAnimation)
        {
            currentTime += currentAnimation->GetTicksPerSecond() * dt;
            std::cout << currentTime << std::endl;
            currentTime = fmod(currentTime, currentAnimation->GetDuration());
            CalculateBoneTransform(&currentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::PlayAnimation(Animation * pAnimation)
    {
        currentAnimation = pAnimation;
        currentTime      = 0.0f;
    }

    void Animator::CalculateBoneTransform(const Animation::AssimpNodeData * node, glm::mat4 parentTransform)
    {
        std::string nodeName      = node->name;
        glm::mat4   nodeTransform = node->transformation;

        Bone * bone = currentAnimation->FindBone(nodeName);

        std::cout << bone << std::endl;

        if (bone)
        {
            bone->Update(currentTime);
            nodeTransform = bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = currentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int       index  = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;

            finalBoneMatrices[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4> Animator::GetFinalBoneMatrices() { return finalBoneMatrices; }
} // namespace sckz