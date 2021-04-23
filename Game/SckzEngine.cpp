#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 854, 480);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    sckz::GraphicsPipeline & p1   = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1   = vkan.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", p1);
    sckz::Light &            l1   = vkan.CreateLight();
    sckz::Light &            l2   = vkan.CreateLight();
    sckz::Entity &           e1   = vkan.CreateEntity(m1);
    sckz::Camera &           cam1 = vkan.CreateCamera(45, 0.1, 10);

    vkan.SetMSAA(VK_SAMPLE_COUNT_8_BIT);

    e1.SetShine(1, 10);
    e1.SetRotation(90, 0, 0);
    e1.SetLocation(0, 1, 0);

    l1.SetColor(0.2, 0.2, 0.2);
    l1.SetLocation(1, 1, 1);
    l1.SetAttenuation(1, 0.01, 0.002);

    l2.SetColor(1, 1, 0);
    l2.SetLocation(0, 0, 0);
    l2.SetAttenuation(1, 0.01, 0.002);

    cam1.SetLocation(0, 5, 0);
    cam1.SetRotation(-90, 0, 0);

    while (!win.QueryClose())
    {

        if (win.QueryKey('y'))
        {
            e1.SetRotation(e1.GetRotation().x, e1.GetRotation().y + (40 * vkan.GetDeltaT()), e1.GetRotation().z);
        }

        if (win.QueryKey('t'))
        {
            e1.SetRotation(e1.GetRotation().x, e1.GetRotation().y - (40 * vkan.GetDeltaT()), e1.GetRotation().z);
        }

        if (win.QueryKey('w'))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y - (1 * vkan.GetDeltaT()), cam1.GetLocation().z);
        }

        if (win.QueryKey('S'))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y + (1 * vkan.GetDeltaT()), cam1.GetLocation().z);
        }

        if (win.QueryKey('a'))
        {
            cam1.SetLocation(cam1.GetLocation().x + (1 * vkan.GetDeltaT()), cam1.GetLocation().y, cam1.GetLocation().z);
        }

        if (win.QueryKey('d'))
        {
            cam1.SetLocation(cam1.GetLocation().x - (1 * vkan.GetDeltaT()), cam1.GetLocation().y, cam1.GetLocation().z);
        }

        if (win.QueryKey('z'))
        {
            cam1.SetRotation(cam1.GetRotation().x,
                             cam1.GetRotation().y,
                             cam1.GetRotation().z - (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('c'))
        {
            cam1.SetRotation(cam1.GetRotation().x,
                             cam1.GetRotation().y,
                             cam1.GetRotation().z + (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('q'))
        {
            cam1.SetRotation(cam1.GetRotation().x - (10 * vkan.GetDeltaT()),
                             cam1.GetRotation().y,
                             cam1.GetRotation().z);
        }

        if (win.QueryKey('e'))
        {
            cam1.SetRotation(cam1.GetRotation().x + (10 * vkan.GetDeltaT()),
                             cam1.GetRotation().y,
                             cam1.GetRotation().z);
        }

        if (win.QueryKey('p'))
        {
            vkan.SetFPS(20);
        }
        if (win.QueryKey('l'))
        {
            vkan.SetFPS(-1);
        }

        if (win.QueryKey(GLFW_KEY_LEFT_SHIFT))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y, cam1.GetLocation().z + (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_SPACE))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y, cam1.GetLocation().z - (1 * vkan.GetDeltaT()));
        }

        win.Update();
        vkan.Update();

        vkan.Render(cam1);
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}