#include "../Include.hpp"
#include "../SckzCore/Vulkan/Combine.hpp"
#include "../SckzCore/Vulkan/Fbo.hpp"
#include "../SckzCore/Vulkan/Filter.hpp"
#include "../SckzCore/Vulkan/Scene.hpp"

namespace sckz
{
    class Bloom
    {
    private:
        Fbo * fbo1;
        Fbo * fbo2;

        Combine * combine;
        Filter *  brightness;
        Filter *  hBlur;
        Filter *  vBlur;

        Scene * scene;

    public:
        void  CreateBloom(Scene & scene);
        Fbo & ApplyBloom();
    };
} // namespace sckz