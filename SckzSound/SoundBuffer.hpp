#pragma once
#include "../Include.hpp"

class SoundBuffer
{
public:
    void CreateSoundBuffer(const char * filename);
    void DestroySoundBuffer();

    ALuint GetBuffer();

private:
    ALuint soundEffectBuffer;
};
