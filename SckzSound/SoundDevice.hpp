#pragma once
#include "../Include.hpp"
#include "SoundBuffer.hpp"

class SoundDevice
{
private:
    ALCdevice *                p_ALCDevice;
    ALCcontext *               p_ALCContext;
    std::vector<SoundBuffer *> buffers;
    glm::vec3                  position = { 0, 0, 0 };
    glm::vec3                  rotation = { 0, 0, 0 };
    glm::vec3                  velocity = { 0, 0, 0 };

public:
    void CreateSoundDevice(const ALCchar * devicename);
    void DestroySoundDevice();

    void      SetListenerPosition(float x, float y, float z);
    void      SetListenerVelocity(float x, float y, float z);
    void      SetListenerRotation(float x, float y, float z);
    glm::vec3 GetListenerPosition();
    glm::vec3 GetListenerVelocity();
    glm::vec3 GetListenerRotation();

    SoundBuffer & CreateSoundBuffer(const char * filename);
};
