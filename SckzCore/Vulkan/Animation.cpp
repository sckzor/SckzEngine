#include "Animation.hpp"

#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace sckz
{
    void Animation::CreateAnimation(const std::string & animationPath, Model * model)
    {
        Assimp::Importer importer;
        const aiScene *  scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration       = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        ReadHeirarchyData(rootNode, scene->mRootNode);
        ReadMissingBones(animation, *model);
    }

    void Animation::DestroyAnimation() { }

    Bone * Animation::FindBone(const std::string & name)
    {
        auto iter
            = std::find_if(bones.begin(), bones.end(), [&](const Bone & bone) { return bone.GetBoneName() == name; });
        if (iter == bones.end())
            return nullptr;
        else
            return &(*iter);
    }

    float Animation::GetTicksPerSecond() { return ticksPerSecond; }

    float Animation::GetDuration() { return duration; }

    const Animation::AssimpNodeData & Animation::GetRootNode() { return rootNode; }

    const std::map<std::string, BoneInfo> & Animation::GetBoneIDMap() { return boneInfoMap; }

    void Animation::ReadMissingBones(const aiAnimation * animation, Model & model)
    {
        int size = animation->mNumChannels;

        auto &   boneInfoMap = model.GetBoneMap();   // getting m_BoneInfoMap from Model class
        uint32_t boneCount   = model.GetBoneCount(); // getting the m_BoneCounter from Model class

        // reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto        channel  = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            bones.push_back(Bone {});
            bones.back().CreateBone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
        }

        this->boneInfoMap = boneInfoMap;
    }

    void Animation::ReadHeirarchyData(AssimpNodeData & dest, const aiNode * src)
    {
        assert(src);

        dest.name           = src->mName.data;
        dest.transformation = BoneInfo::Mat4x4FromAssimp(src->mTransformation);
        dest.childrenCount  = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

} // namespace sckz