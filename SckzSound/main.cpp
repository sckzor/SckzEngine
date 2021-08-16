#include "SoundBuffer.hpp"
#include "SoundDevice.hpp"
#include "SoundSource.hpp"

#include <iostream>

int main()
{
    SoundDevice mysounddevice;
    mysounddevice.CreateSoundDevice(nullptr);

    SoundBuffer & sound1 = mysounddevice.CreateSoundBuffer("Polaroid.wav");

    SoundSource mySpeaker;
    mySpeaker.CreateSource();
    mySpeaker.SetPosition(0, 0, 0);

    mySpeaker.Play(sound1);
    mySpeaker.SetGain(0.5);
    bool growing;

    while (mySpeaker.IsPlaying())
    {

        if (mysounddevice.GetListenerPosition().x >= 10)
            growing = false;

        else if (mysounddevice.GetListenerPosition().x <= -10)
            growing = true;

        if (growing)
        {
            mysounddevice.SetListenerPosition(mysounddevice.GetListenerPosition().x + 0.0001,
                                              mysounddevice.GetListenerPosition().y,
                                              mysounddevice.GetListenerPosition().z);
        }
        else
        {
            mysounddevice.SetListenerPosition(mysounddevice.GetListenerPosition().x - 0.0001,
                                              mysounddevice.GetListenerPosition().y,
                                              mysounddevice.GetListenerPosition().z);
        }
    }

    mySpeaker.DestroySource();
    mysounddevice.DestroySoundDevice();

    return 0;
}