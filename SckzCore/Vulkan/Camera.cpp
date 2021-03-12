#include "Camera.hpp"

namespace sckz
{
    void Camera::CreateCamera(float fov, float near, float far, VkExtent2D extent)
    {
        this->near   = near;
        this->far    = far;
        this->fov    = fov;
        this->extent = extent;
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

        projection = glm::perspective(glm::radians(fov), extent.width / (float)extent.height, near, far);
        projection[1][1] *= -1;
    }

    glm::mat4 Camera::GetProjection() { return projection; }

    glm::mat4 Camera::GetView() { return view; }

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