#include "LightSort.hpp"

namespace sckz
{
    std::vector<std::pair<Light *, float>> LightSort::GetNearestLights(std::vector<Light *> lights, glm::vec3 location)
    {
        std::vector<std::pair<Light *, float>> distances;
        for (uint32_t i = 0; i < lights.size(); i++)
        {
            float distance = glm::distance(lights[i]->GetLocation(), location);
            distances.push_back({ lights[i], distance });
        }

        QuickSort(distances, 0, distances.size() - 1);

        return distances;
    }

    void LightSort::Swap(std::vector<std::pair<Light *, float>> & distances, int32_t i, int32_t j)
    {
        std::pair<Light *, float> temp = distances[i];
        distances[i]                   = distances[j];
        distances[j]                   = temp;
    }

    uint32_t LightSort::Partition(std::vector<std::pair<Light *, float>> & distances, int32_t l, int32_t r)
    {
        float   pivot = distances[r].second;
        int32_t i     = l - 1;

        for (uint32_t j = l; j < r; j++)
        {
            if (distances[j].second < pivot)
            {
                i++;
                Swap(distances, i, j);
            }
        }
        Swap(distances, i + 1, r);
        return i + 1;
    }

    void LightSort::QuickSort(std::vector<std::pair<Light *, float>> & distances, int32_t l, int32_t r)
    {
        if (l < r)
        {
            int32_t pi = Partition(distances, l, r);
            QuickSort(distances, l, pi - 1);
            QuickSort(distances, pi + 1, r);
        }
    }
} // namespace sckz