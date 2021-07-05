#include "../../Include.hpp"

namespace sckz
{
    void InitRand() { srand(time(0)); }

    float GetRandomFloat(float max, float min)
    {
        float dec = (float)rand() / RAND_MAX;
        dec       = (dec * (max - min)) - max;
        return dec;
    }
} // namespace sckz