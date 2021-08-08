#pragma once

namespace sckz
{
    class Timer
    {
    private:
        double currentTime;

    public:
        void StartTimer();
        void ResetTimer();

        void Update(double deltaT);

        double GetCurrentTime();
    };
} // namespace sckz