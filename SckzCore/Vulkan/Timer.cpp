#include "Timer.hpp"

namespace sckz
{
    void Timer::ResetTimer() { currentTime = 0; }

    void Timer::Update(double deltaT) { currentTime += deltaT; }

    double Timer::GetCurrentTime() { return currentTime; }
} // namespace sckz