#include "SoundDevice.hpp"

#include <AL/al.h>
#include <stdio.h>

void SoundDevice::CreateSoundDevice(const ALCchar * devicename)
{
    p_ALCDevice = alcOpenDevice(nullptr); // nullptr = get default device
    if (!p_ALCDevice)
        throw("failed to get sound device");

    p_ALCContext = alcCreateContext(p_ALCDevice, nullptr); // create context
    if (!p_ALCContext)
        throw("Failed to set sound context");

    if (!alcMakeContextCurrent(p_ALCContext)) // make context current
        throw("failed to make context current");

    const ALCchar * name = nullptr;
    if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(p_ALCDevice) != AL_NO_ERROR)
        name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
    printf("Opened \"%s\"\n", name);

    alListener3f(AL_POSITION, position.x, position.y, position.z);
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void SoundDevice::SetListenerPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void SoundDevice::SetListenerVelocity(float x, float y, float z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
    alListener3f(AL_POSITION, velocity.x, velocity.y, velocity.z);
}

glm::vec3 SoundDevice::GetListenerRotation() { return rotation; }

void SoundDevice::SetListenerRotation(float x, float y, float z)
{
    glm::vec3 at = glm::vec3(0, 1, 0);
    rotation.x   = x;
    rotation.y   = y;
    rotation.z   = z;

    at = glm::rotate(at, glm::radians(rotation.z), glm::vec3(0, 0, -1));

    ALfloat dir[6] = { 0.0f, 0.0f, 1.0f, at.x, at.y, at.z };
    alListenerfv(AL_ORIENTATION, dir);
    // alListener3f(AL_ORIENTATION, rotation.x, rotation.y, rotation.z);
}

glm::vec3 SoundDevice::GetListenerPosition() { return position; }

glm::vec3 SoundDevice::GetListenerVelocity() { return velocity; }

void SoundDevice::DestroySoundDevice()
{
    for (uint32_t i = 0; i < buffers.size(); i++)
    {
        buffers[i]->DestroySoundBuffer();
        delete buffers[i];
    }

    if (!alcMakeContextCurrent(nullptr))
        throw("failed to set context to nullptr");

    alcDestroyContext(p_ALCContext);
    if (p_ALCContext)
        std::cout << "failed to unset during close" << std::endl;

    if (!alcCloseDevice(p_ALCDevice))
        throw("failed to close sound device");
}

SoundBuffer & SoundDevice::CreateSoundBuffer(const char * filename)
{
    SoundBuffer * buffer = new SoundBuffer();
    buffer->CreateSoundBuffer(filename);
    buffers.push_back(buffer);
    return *buffer;
}