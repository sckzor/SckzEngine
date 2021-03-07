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

    void Camera::DestroyCamera() { }

    void Camera::UpdateMatrix()
    {
        view       = glm::lookAt(location, location + rotation, glm::vec3(0.0f, 0.0f, 1.0f));
        projection = glm::perspective(glm::radians(fov), extent.width / (float)extent.height, near, far);
        projection[1][1] *= -1;
    }

    glm::mat4 Camera::GetProjection() { return projection; }

    glm::mat4 Camera::GetView() { return view; }

    glm::vec3 Camera::GetRoatation() { return rotation; }

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

    void Camera::SetRoatation(float x, float y, float z)
    {
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
        UpdateMatrix();
    }
} // namespace sckz