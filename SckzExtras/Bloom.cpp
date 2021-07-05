#include "Bloom.hpp"

namespace sckz
{
    void Bloom::CreateBloom(Scene & scene)
    {
        this->scene = &scene;

        brightness = &scene.CreateFilter("Resources/fbo_fragment_bright.spv");
        hBlur      = &scene.CreateFilter("Resources/fbo_fragment_gaussian_horizontal.spv");
        vBlur      = &scene.CreateFilter("Resources/fbo_fragment_gaussian_vertical.spv");
        combine    = &scene.CreateCombine("Resources/fbo_fragment_combine_additive.spv");
    }

    Fbo & Bloom::ApplyBloom()
    {
        fbo1 = &brightness->FilterFbo(scene->GetRenderedImage());
        fbo2 = &hBlur->FilterFbo(*fbo1);
        fbo1 = &vBlur->FilterFbo(*fbo2);

        fbo2 = &combine->CombineFbos(*fbo1, scene->GetRenderedImage());

        return *fbo2;
    }
} // namespace sckz