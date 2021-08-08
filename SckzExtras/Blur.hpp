#pragma once
#include "../Include.hpp"
#include "../SckzCore/Vulkan/Fbo.hpp"
#include "../SckzCore/Vulkan/Filter.hpp"
#include "../SckzCore/Vulkan/Scene.hpp"

namespace sckz
{
    class Blur
    {
    private:
        Fbo * fbo1;
        Fbo * fbo2;

        Filter * hBlur;
        Filter * vBlur;

        Scene * scene;

    public:
        void  CreateBlur(Scene & scene);
        Fbo & ApplyBlur();
    };
} // namespace sckz