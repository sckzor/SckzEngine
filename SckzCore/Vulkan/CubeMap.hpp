#include "../../Include.hpp"
#include "Image.hpp"
#include "Scene.hpp"

namespace sckz
{
    class CubeMap
    {
    private:
        Image cubeMapTexture;

    public:
        void CreateCubeMap();
        void DestroyCubeMap();

        void RenderCubeMap(Scene & scene, glm::vec3 location);
    };
} // namespace sckz