#pragma once
#include "../Include.hpp"
#include "SoundBuffer.hpp"

class SoundSource
{
public:
    void CreateSource();
    void DestroySource();

    void Play(SoundBuffer & buffer);
    bool IsPlaying();

    void Pause();
    void Resume();
    void Stop();

    void SetPosition(float x, float y, float z);
    void SetVelocity(float x, float y, float z);
    void SetGain(float gain);
    void SetPitch(float pitch);
    void SetShouldLoop(bool shouldLoop);

    glm::vec3 GetPosition();
    glm::vec3 GetVelocity();
    float     GetGain();
    float     GetPitch();
    bool      GetShouldLoop();

private:
    ALuint    source;
    float     pitch     = 1.f;
    float     gain      = 1.f;
    glm::vec3 position  = { -10, 0, 0 };
    glm::vec3 velocity  = { 0, 0, 0 };
    bool      loopSound = false;
    ALuint    buffer    = 0;

    ALint state = AL_PLAYING;
};
