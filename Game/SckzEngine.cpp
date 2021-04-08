#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    sckz::GraphicsPipeline & p1   = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1   = vkan.CreateModel("Resources/room.obj", "Resources/RoomTextureAtlas.png", p1);
    sckz::Model &            m2   = vkan.CreateModel("Resources/flare.obj", "Resources/placeholder.png", p1);
    sckz::Light &            l1   = vkan.CreateLight();
    sckz::Light &            l2   = vkan.CreateLight();
    sckz::Entity &           e1   = vkan.CreateEntity(m1);
    sckz::Entity &           e2   = vkan.CreateEntity(m2);
    sckz::Entity &           e3   = vkan.CreateEntity(m2);
    sckz::Camera &           cam1 = vkan.CreateCamera(45, 0.1, 10);

    e1.SetShine(0, 0.1);
    e1.SetRotation(90, 0, 0);
    e1.SetLocation(0, 1, 0);

    e2.SetShine(0, 0.1);
    e2.SetRotation(0, 0, 0);
    e2.SetLocation(0, 0.5, -1);
    e2.SetScale(0.1, 0.1, 0.1);

    e3.SetShine(0, 0.1);
    e3.SetRotation(0, 0, 0);
    e3.SetLocation(10, 10, 20);
    e3.SetScale(0.1, 0.1, 0.1);

    l1.SetColor(1, 0, 0);
    l1.SetLocation(10, 10, 20);
    l1.SetAttenuation(1, 0, 0);

    l2.SetColor(0, 1, 0);
    l2.SetLocation(0, 1, -2.5);
    l2.SetAttenuation(1, 0, 0);

    cam1.SetLocation(0, 5, 0);
    cam1.SetRotation(-90, 0, 0);

    while (!win.QueryClose())
    {
        std::cout << "[CAMERA LOCATION] X: " << cam1.GetLocation().x << " Y: " << cam1.GetLocation().y
                  << " Z: " << cam1.GetLocation().z << std::endl;
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

        if (win.QueryKey(GLFW_KEY_LEFT_SHIFT))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y, cam1.GetLocation().z + (1 * vkan.GetDeltaT()));
        }

        if (win.QueryKey(GLFW_KEY_SPACE))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y, cam1.GetLocation().z - (1 * vkan.GetDeltaT()));
        }

        vkan.Render(cam1);

        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}