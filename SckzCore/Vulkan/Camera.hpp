#pragma once
#include "../../Include.hpp"
#include "../../SckzSound/SoundDevice.hpp"
#include "../../SckzSound/SoundSource.hpp"

namespace sckz
{
    class Camera
    {
    private:
        glm::mat4 view;
        glm::mat4 projection;

        glm::mat4 cubeMapViews[CUBEMAP_SIDES];

        glm::vec3 location;
        glm::vec3 rotation;

        float fov;
        float near, far;

        bool       isCubeMap;
        VkExtent2D extent;

        SoundDevice * soundDevice = nullptr;
        SoundSource * musicSource = nullptr;
        SoundSource * sfxSource   = nullptr;

    public:
        void CreateCamera(float fov, float near, float far, VkExtent2D extent, bool isCubeMap);
        void UpdateExtent(VkExtent2D extent);
        void DestroyCamera();

        void SetLocation(float x, float y, float z);
        void SetRotation(float x, float y, float z);
        void SetFOV(float fov);

        void SetListener(SoundDevice * soundDevice);
        void SetMusicSource(SoundSource * musicSource);
        void SetSfxSource(SoundSource * sfxSource);

    public:
        glm::mat4 GetView();
        glm::mat4 GetCubeMapView(uint32_t side);
        glm::mat4 GetProjection();

        glm::vec3 GetRotation();
        glm::vec3 GetLocation();

        SoundDevice * GetSoundDevice();

    private:
        void UpdateMatrix();
    };
} // namespace sckz