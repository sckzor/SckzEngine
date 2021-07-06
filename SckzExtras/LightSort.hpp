#include "../Include.hpp"
#include "../SckzCore/Vulkan/Light.hpp"

namespace sckz
{
    class LightSort
    {
    public:
        static std::vector<std::pair<Light *, float>> GetNearestLights(std::vector<Light *> lights, glm::vec3 location);

    private:
        static uint32_t Partition(std::vector<std::pair<Light *, float>> & distances, int32_t l, int32_t r);

        static void Swap(std::vector<std::pair<Light *, float>> & distances, int32_t i, int32_t j);

        static void QuickSort(std::vector<std::pair<Light *, float>> & distances, int32_t l, int32_t r);
    };
} // namespace sckz