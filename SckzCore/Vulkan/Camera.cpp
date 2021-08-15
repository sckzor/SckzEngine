#include "Camera.hpp"

namespace sckz
{
    void Camera::CreateCamera(float fov, float near, float far, VkExtent2D extent, bool isCubeMap)
    {
        this->near      = near;
        this->far       = far;
        this->fov       = fov;
        this->extent    = extent;
        this->isCubeMap = isCubeMap;
    }

    void Camera::UpdateExtent(VkExtent2D extent)
    {
        this->extent = extent;
        UpdateMatrix();
    }

    void Camera::SetListener(SoundDevice * soundDevice) { this->soundDevice = soundDevice; }
    void Camera::SetMusicSource(SoundSource * musicSource) { this->musicSource = musicSource; }
    void Camera::SetSfxSource(SoundSource * sfxSource) { this->sfxSource = sfxSource; }

    void Camera::DestroyCamera() { }

    void Camera::UpdateMatrix()
    {
        if (isCubeMap)
        {
            cubeMapViews[0] = glm::mat4x4(1.0f);
            cubeMapViews[0] = glm::rotate(cubeMapViews[0], glm::radians(90.0f), glm::vec3(0, 1, 0));
            cubeMapViews[0] = glm::translate(cubeMapViews[0], location);

            cubeMapViews[1] = glm::mat4x4(1.0f);
            cubeMapViews[1] = glm::rotate(cubeMapViews[1], glm::radians(270.0f), glm::vec3(0, 1, 0));
            cubeMapViews[1] = glm::translate(cubeMapViews[1], location);

            cubeMapViews[2] = glm::mat4x4(1.0f);
            cubeMapViews[2] = glm::rotate(cubeMapViews[2], glm::radians(270.0f), glm::vec3(1, 0, 0));
            cubeMapViews[2] = glm::translate(cubeMapViews[2], location);

            cubeMapViews[3] = glm::mat4x4(1.0f);
            cubeMapViews[3] = glm::rotate(cubeMapViews[3], glm::radians(90.0f), glm::vec3(1, 0, 0));
            cubeMapViews[3] = glm::translate(cubeMapViews[3], location);

            cubeMapViews[4] = glm::mat4x4(1.0f);
            cubeMapViews[4] = glm::rotate(cubeMapViews[4], glm::radians(180.0f), glm::vec3(0, 1, 0));
            cubeMapViews[4] = glm::translate(cubeMapViews[4], location);

            cubeMapViews[5] = glm::mat4x4(1.0f);
            cubeMapViews[5] = glm::rotate(cubeMapViews[5], glm::radians(0.0f), glm::vec3(1, 0, 0));
            cubeMapViews[5] = glm::translate(cubeMapViews[5], location);

            projection = glm::perspective(glm::radians(90.0f), 1.0f, near, far);
        }
        else
        {
            view = glm::mat4x4(1.0f);
            view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1, 0, 0));
            view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0, 1, 0));
            view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            view = glm::translate(view, location);

            projection = glm::perspective(glm::radians(fov), extent.width / (float)extent.height, near, far);
        }

        projection[1][1] *= -1;
    }

    glm::mat4 Camera::GetProjection() { return projection; }

    glm::mat4 Camera::GetView() { return view; }

    glm::mat4 Camera::GetCubeMapView(uint32_t side) { return cubeMapViews[side]; }

    glm::vec3 Camera::GetRotation() { return rotation; }

    glm::vec3 Camera::GetLocation() { return location; }

    void Camera::SetFOV(float fov)
    {
        this->fov = fov;
        UpdateMatrix();
    }

    void Camera::SetLocation(float x, float y, float z)
    {
        location.x = x;
        location.y = y;
        location.z = z;

        if (soundDevice != nullptr)
        {
            soundDevice->SetListenerPosition(x, y, z);
        }

        if (musicSource != nullptr)
        {
            musicSource->SetPosition(x, y, z);
        }

        if (sfxSource != nullptr)
        {
            sfxSource->SetPosition(x, y, z);
        }

        UpdateMatrix();
    }

    SoundDevice * Camera::GetSoundDevice() { return soundDevice; }

    void Camera::SetRotation(float x, float y, float z)
    {
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;

        if (soundDevice != nullptr)
        {
            soundDevice->SetListenerRotation(x, y, z);
        }

        UpdateMatrix();
    }
} // namespace sckz