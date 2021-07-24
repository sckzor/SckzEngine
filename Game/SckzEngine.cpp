#include "SckzEngine.hpp"

int main()
{
    sckz::InitRand();

    sckz::Window win;
    win.CreateWindow("SckzEngine", 700, 500);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    win.SetIcon("Resources/icon.png");

    // vkan.SetFPS(-1);

    sckz::Scene & s1 = vkan.CreateScene();
    sckz::Scene & s2 = vkan.CreateScene();

    sckz::GraphicsPipeline & f2 = vkan.CreateFBOPipeline("Resources/fbo_fragment_invert.spv");
    sckz::GraphicsPipeline & f1 = vkan.CreateFBOPipeline("Resources/fbo_fragment_normal.spv");

    sckz::GraphicsPipeline & p1 = s1.CreatePipeline("Resources/simple_vertex.spv",
                                                    "Resources/simple_fragment.spv",
                                                    "Resources/cubemap_render_vertex.spv",
                                                    "Resources/cubemap_render_fragment.spv");

    sckz::Model & m1 = s1.CreateModel("Resources/barrel.obj",
                                      "Resources/barrelColor.png",
                                      "Resources/barrelNormal.png",
                                      "Resources/barrelSpecular.png",
                                      p1);
    sckz::Model & m3 = s1.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", nullptr, nullptr, p1);
    sckz::Light & l1 = s1.CreateLight(true);

    sckz::ParticleSystem & pa1 = s1.CreateParticleSystem(1000, "Resources/fireParticles.png", 4, 4, 15);

    sckz::GraphicsPipeline & p2 = s2.CreatePipeline("Resources/simple_vertex.spv",
                                                    "Resources/simple_fragment.spv",
                                                    "Resources/cubemap_render_vertex.spv",
                                                    "Resources/cubemap_render_fragment.spv");

    sckz::Model & m2 = s2.CreateModel("Resources/barrel.obj",
                                      "Resources/barrelColor.png",
                                      "Resources/barrelNormal.png",
                                      "Resources/barrelSpecular.png",
                                      p2);

    sckz::Light & l2 = s1.CreateLight(true);

    sckz::Entity & e1 = s1.CreateEntity(m1, true);
    sckz::Entity & e3 = s1.CreateEntity(m3, false);
    sckz::Camera & c1 = s1.CreateCamera(45, 0.1, 100);

    // sckz::Entity & e2 = s2.CreateEntity(m2);
    sckz::Camera & c2 = s2.CreateCamera(45, 0.1, 100);

    sckz::Gui & gui = vkan.CreateGUI("Resources/icon.png");

    sckz::Fbo & fbo = s1.CreateFbo();

    sckz::Bloom bloom;
    bloom.CreateBloom(s1);

    gui.SetScale(200, 200);
    gui.SetLocation(500, 300);
    // gui.SetRotationPoint(100, 100);

    e1.SetShine(1, 10);
    e1.SetRotation(90, 0, 0);
    e1.SetLocation(0, 0, 0);
    e1.SetScale(0.1, 0.1, 0.1);
    e1.SetReflectRefractValues(1 / 1.33, 0.6);

    l1.SetColor(0, 0.5, 1);
    l1.SetLocation(0, 0, 0);
    l1.SetAttenuation(1, 0.01, 0.002);
    l1.SetDirection(0, -1, 0);
    l1.SetCutoff(12.5, 17.5);

    l2.SetColor(1, 0.5, 0);
    l2.SetLocation(0, 0, 0);
    l2.SetAttenuation(1, 0.01, 0.002);
    l2.SetDirection(-1, 0, 0);
    l2.SetCutoff(12.5, 17.5);

    // e2.SetShine(1, 10);
    // e2.SetRotation(90, 0, 0);
    // e2.SetLocation(0, 0, 0);
    // e2.SetScale(0.1, 0.1, 0.1);

    e3.SetShine(1, 30);
    e3.SetRotation(90, 0, 0);
    e3.SetLocation(0, 0, 0);
    e3.SetReflectRefractValues(1, 0);

    c1.SetLocation(0, 0, 0);
    c1.SetRotation(-90, 0, 0);
    c2.SetLocation(0, 5, 0);
    c2.SetRotation(-90, 0, 0);

    while (!win.QueryClose())
    {
        if (win.QueryKey('w'))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y - (1 * vkan.GetDeltaT()), c1.GetLocation().z);
        }

        if (win.QueryKey('S'))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y + (1 * vkan.GetDeltaT()), c1.GetLocation().z);
        }

        if (win.QueryKey('a'))
        {
            c1.SetLocation(c1.GetLocation().x + (1 * vkan.GetDeltaT()), c1.GetLocation().y, c1.GetLocation().z);
        }

        if (win.QueryKey('d'))
        {
            c1.SetLocation(c1.GetLocation().x - (1 * vkan.GetDeltaT()), c1.GetLocation().y, c1.GetLocation().z);
        }

        if (win.QueryKey('z'))
        {
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z - (30 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('c'))
        {
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z + (30 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('q'))
        {
            c1.SetRotation(c1.GetRotation().x - (30 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey('e'))
        {
            c1.SetRotation(c1.GetRotation().x + (30 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey(GLFW_KEY_LEFT_SHIFT))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z + (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_SPACE))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z - (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_TAB))
        {
            pa1.AddParticle(0, 0, 0, sckz::GetRandomFloat(1, -1), sckz::GetRandomFloat(1, -1), 3);
        }

        if (win.QueryKey('p'))
        {
            vkan.SetFPS(30);
        }

        if (win.QueryKey('l'))
        {
            vkan.SetFPS(-1);
        }

        if (win.QueryKey('g'))
        {
            s1.SetMSAA(8);
            s2.SetMSAA(8);
        }

        if (win.QueryKey('u'))
        {
            s2.Render(c2, vkan.GetDeltaT(), fbo);

            // sckz::Fbo & filteredImage = fil.FilterFbo(s2.GetRenderedImage());

            if (win.QueryKey('y'))
            {
                vkan.Present(s2.GetRenderedImage(), f2);
            }
            else
            {
                vkan.Present(s2.GetRenderedImage(), f1);
            }
        }
        else
        {
            s1.Render(c1, vkan.GetDeltaT(), fbo);

            sckz::Fbo & bloomFbo = bloom.ApplyBloom();

            if (win.QueryKey('y'))
            {

                vkan.Present(s1.GetRenderedImage(), f1);
            }
            else
            {
                vkan.Present(bloomFbo, f1);
            }
        }

        if (win.QueryKey('5'))
        {
            l1.SetLocation(l1.GetLocation().x, l1.GetLocation().y + (vkan.GetDeltaT()), l1.GetLocation().z);
        }
        if (win.QueryKey('4'))
        {
            l1.SetLocation(l1.GetLocation().x, l1.GetLocation().y - (vkan.GetDeltaT()), l1.GetLocation().z);
        }
        if (win.QueryKey('1'))
        {
            l1.SetCutoff(l1.GetCutoff(), l1.GetOuterCutoff() + (vkan.GetDeltaT()));
        }
        if (win.QueryKey('2'))
        {
            l1.SetCutoff(l1.GetCutoff() + (vkan.GetDeltaT()), l1.GetOuterCutoff());
        }

        if (win.QueryKey('6'))
        {
            e3.SetRotation(e3.GetRotation().x, e3.GetRotation().y - (10 * vkan.GetDeltaT()), e3.GetRotation().z);
        }

        if (win.QueryKey('7'))
        {
            e3.SetRotation(e3.GetRotation().x, e3.GetRotation().y + (10 * vkan.GetDeltaT()), e3.GetRotation().z);
        }

        win.Update();
    }

    s1.DestroyScene();
    s2.DestroyScene();
    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}