#pragma once
#include "../Include.hpp"

#include <sndfile.h>
class MusicBuffer
{
public:
    void Play();
    // void Pause();
    // void Stop();

    void UpdateBufferStream();

    ALint getSource();

    void  SetGain(float gain);
    float GetGain();

    void CreateMusicBuffer(const char * filename);
    void DestroyMusicBuffer();

private:
    ALuint           p_Source;
    static const int BUFFER_SAMPLES = 8192;
    static const int NUM_BUFFERS    = 4;
    ALuint           p_Buffers[NUM_BUFFERS];
    SNDFILE *        p_SndFile;
    SF_INFO          p_Sfinfo;
    short *          p_Membuf;
    ALenum           p_Format;

    float gain = 0;
};
