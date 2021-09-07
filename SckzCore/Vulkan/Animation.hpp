#pragma once
#include "../../Include.hpp"
#include "Bone.hpp"
#include "BoneInfo.hpp"
#include "Model.hpp"

namespace sckz
{

    class Animation
    {
    public:
        struct AssimpNodeData
        {
            glm::mat4                   transformation;
            std::string                 name;
            int                         childrenCount;
            std::vector<AssimpNodeData> children;
        };

    public:
        void CreateAnimation(const std::string & animationPath, Model * model);

        void DestroyAnimation();

        Bone * FindBone(const std::string & name);

        float GetTicksPerSecond();

        float GetDuration();

        const AssimpNodeData & GetRootNode();

        const std::map<std::string, BoneInfo> & GetBoneIDMap();

    private:
        void ReadMissingBones(const aiAnimation * animation, Model & model);

        void ReadHeirarchyData(AssimpNodeData & dest, const aiNode * src);

        float                           duration;
        int                             ticksPerSecond;
        std::vector<Bone>               bones;
        AssimpNodeData                  rootNode;
        std::map<std::string, BoneInfo> boneInfoMap;
    };
} // namespace sckz