#include "SoundSource.hpp"

#include <iostream>

void SoundSource::CreateSource()
{
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, pitch);
    alSourcef(source, AL_GAIN, gain);
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    alSourcei(source, AL_LOOPING, loopSound);
    alSourcei(source, AL_BUFFER, buffer);
    alDistanceModel(AL_LINEAR_DISTANCE);
    alSourcef(source, AL_ROLLOFF_FACTOR, 1.0);
    alSourcef(source, AL_REFERENCE_DISTANCE, 1.0);
    alSourcef(source, AL_MAX_DISTANCE, 2.0);
}

void SoundSource::Pause() { alSourcePause(source); }

void SoundSource::Resume() { alSourcePlay(source); }

void SoundSource::Stop() { alSourceStop(source); }

void SoundSource::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}

void SoundSource::SetVelocity(float x, float y, float z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void SoundSource::SetGain(float gain)
{
    this->gain = gain;
    alSourcef(source, AL_GAIN, gain);
}

void SoundSource::SetPitch(float pitch)
{
    this->pitch = pitch;
    alSourcef(source, AL_PITCH, pitch);
}

void SoundSource::SetShouldLoop(bool shouldLoop)
{
    this->loopSound = shouldLoop;
    alSourcei(source, AL_LOOPING, loopSound);
}

glm::vec3 SoundSource::GetPosition() { return position; }
glm::vec3 SoundSource::GetVelocity() { return velocity; }
float     SoundSource::GetGain() { return gain; }
float     SoundSource::GetPitch() { return pitch; }
bool      SoundSource::GetShouldLoop() { return loopSound; }

void SoundSource::DestroySource()
{
    Stop();
    alDeleteSources(1, &source);
}

void SoundSource::Play(SoundBuffer & buffer)
{
    if (buffer.GetBuffer() != this->buffer)
    {
        this->buffer = buffer.GetBuffer();
        alSourcei(source, AL_BUFFER, (ALint)this->buffer);
    }

    alSourcePlay(source);
}

bool SoundSource::IsPlaying()
{

    if (state == AL_PLAYING && alGetError() == AL_NO_ERROR)
    {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
    }
    else
    {
        std::cout << "Playing Sound Failed" << std::endl;
    }

    return state == AL_PLAYING ? true : false;
}