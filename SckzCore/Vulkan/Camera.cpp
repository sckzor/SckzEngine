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

    void Camera::DestroyCamera() { }

    void Camera::UpdateMatrix()
    {
        view = glm::mat4x4(1.0f);
        view = glm::rotate(view, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        view = glm::rotate(view, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        view = glm::rotate(view, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        view = glm::translate(view, location);

        projection        = glm::perspective(glm::radians(fov), extent.width / (float)extent.height, near, far);
        cubeMapProjection = glm::perspective(glm::radians(90.0f), 1.0f, near, far);

        projection[1][1] *= -1;
        cubeMapProjection[1][1] *= -1;
    }

    glm::mat4 Camera::GetProjection() { return projection; }

    glm::mat4 Camera::GetView() { return view; }

    glm::mat4 Camera::GetCubeMapView(uint32_t side, glm::vec3 location)
    {
        cubeMapViews[0] = glm::mat4x4(1.0f);
        cubeMapViews[0] = glm::rotate(cubeMapViews[0], glm::radians(90.0f), glm::vec3(1, 0, 0));
        cubeMapViews[0] = glm::translate(cubeMapViews[0], location);

        for (uint32_t i = 0; i < 4; i++)
        {
            cubeMapViews[i + 1] = glm::mat4x4(1.0f);
            cubeMapViews[i + 1] = glm::rotate(cubeMapViews[i + 1], glm::radians(90.0f * i), glm::vec3(0, 1, 0));
            cubeMapViews[i + 1] = glm::translate(cubeMapViews[i + 1], location);
        }

        cubeMapViews[5] = glm::mat4x4(1.0f);
        cubeMapViews[5] = glm::rotate(cubeMapViews[5], glm::radians(270.0f), glm::vec3(1, 0, 0));
        cubeMapViews[5] = glm::translate(cubeMapViews[5], location);
        return cubeMapViews[side];
    }
    glm::mat4 Camera::GetCubeMapProjection() { return cubeMapProjection; }

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
        UpdateMatrix();
    }

    void Camera::SetRotation(float x, float y, float z)
    {
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
        UpdateMatrix();
    }
} // namespace sckz