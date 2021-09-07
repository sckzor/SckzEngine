#pragma once
#include "../../Include.hpp"
#include "Animation.hpp"

namespace sckz
{
    class Animator
    {
    public:
        void CreateAnimator(Animation * Animation);

        void UpdateAnimation(float dt);

        void PlayAnimation(Animation * pAnimation);

        void CalculateBoneTransform(const Animation::AssimpNodeData * node, glm::mat4 parentTransform);

        std::vector<glm::mat4> GetFinalBoneMatrices();

    private:
        std::vector<glm::mat4> finalBoneMatrices;
        Animation *            currentAnimation;
        float                  currentTime;
        float                  deltaTime;
    };
} // namespace sckz