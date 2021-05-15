#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 854, 480);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    sckz::Scene & s1 = vkan.CreateScene();
    sckz::Scene & s2 = vkan.CreateScene();

    sckz::GraphicsPipeline & f2 = vkan.CreateFBOPipeline("Resources/fbo_fragment_invert.spv");
    sckz::GraphicsPipeline & f1 = vkan.CreateFBOPipeline("Resources/fbo_fragment_normal.spv");

    sckz::GraphicsPipeline & p1 = s1.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1 = s1.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", p1);
    sckz::Light &            l1 = s1.CreateLight();

    sckz::GraphicsPipeline & p2 = s2.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m2 = s2.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", p1);
    sckz::Light &            l2 = s2.CreateLight();

    sckz::Entity & e1 = s1.CreateEntity(m1);
    sckz::Camera & c1 = s1.CreateCamera(45, 0.1, 10);

    sckz::Entity & e2 = s2.CreateEntity(m2);
    sckz::Camera & c2 = s2.CreateCamera(45, 0.1, 10);

    // s1.SetMSAA(-1);

    e1.SetShine(1, 10);
    e1.SetRotation(90, 0, 0);
    e1.SetLocation(0, 1, 0);

    l1.SetColor(1, 1, 0);
    l1.SetLocation(0, 0, 0);
    l1.SetAttenuation(1, 0.01, 0.002);

    e2.SetShine(1, 10);
    e2.SetRotation(90, 0, 0);
    e2.SetLocation(0, 1, 0);

    l2.SetColor(0.2, 0.2, 0.2);
    l2.SetLocation(1, 1, 1);
    l2.SetAttenuation(1, 0.01, 0.002);

    c1.SetLocation(0, 5, 0);
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
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z - (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('c'))
        {
            c1.SetRotation(c1.GetRotation().x, c1.GetRotation().y, c1.GetRotation().z + (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('q'))
        {
            c1.SetRotation(c1.GetRotation().x - (10 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey('e'))
        {
            c1.SetRotation(c1.GetRotation().x + (10 * vkan.GetDeltaT()), c1.GetRotation().y, c1.GetRotation().z);
        }

        if (win.QueryKey(GLFW_KEY_LEFT_SHIFT))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z + (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_SPACE))
        {
            c1.SetLocation(c1.GetLocation().x, c1.GetLocation().y, c1.GetLocation().z - (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('p'))
        {
            vkan.SetFPS(20);
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

        win.Update();
        vkan.Update();

        if (win.QueryKey('u'))
        {
            s2.Render(c2);

            if (win.QueryKey('y'))
            {
                vkan.Present(s2, f1);
            }
            else
            {
                vkan.Present(s2, f2);
            }
        }
        else
        {
            s1.Render(c1);

            if (win.QueryKey('y'))
            {
                vkan.Present(s1, f1);
            }
            else
            {
                vkan.Present(s1, f2);
            }
        }
    }

    s1.DestroyScene();
    s2.DestroyScene();
    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}