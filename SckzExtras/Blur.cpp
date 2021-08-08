#include "Blur.hpp"

namespace sckz
{
    void Blur::CreateBlur(Scene & scene)
    {
        this->scene = &scene;

        hBlur = &scene.CreateFilter("Resources/fbo_fragment_gaussian_horizontal.spv");
        vBlur = &scene.CreateFilter("Resources/fbo_fragment_gaussian_vertical.spv");
    }

    Fbo & Blur::ApplyBlur()
    {
        fbo2 = &hBlur->FilterFbo(scene->GetRenderedImage());
        fbo1 = &vBlur->FilterFbo(*fbo2);

        return *fbo1;
    }
} // namespace sckz