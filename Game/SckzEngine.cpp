#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    // sckz::GraphicsPipeline & p1 = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/blue_fragment.spv");
    sckz::GraphicsPipeline & p2   = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1   = vkan.CreateModel("Resources/flare.obj", "Resources/placeholder.png", p2);
    sckz::Model &            m2   = vkan.CreateModel("Resources/viking_room.obj", "Resources/viking_room.png", p2);
    sckz::Light &            l1   = vkan.CreateLight();
    sckz::Entity &           e1   = vkan.CreateEntity(m1);
    sckz::Entity &           e2   = vkan.CreateEntity(m1);
    sckz::Camera &           cam1 = vkan.CreateCamera(45, 0.1, 10);
    sckz::Camera &           cam2 = vkan.CreateCamera(45, 0.1, 10);
    e1.SetLocation(0, 1, 0);
    cam1.SetLocation(0, 5, 0);
    cam2.SetLocation(0, 8, 0);
    cam1.SetRotation(-90, 0, 0);
    cam2.SetRotation(-90, 0, 0);

    while (!win.QueryClose())
    {
        // std::cout << vkan.GetDeltaT() << std::endl;
        // std::cout << e2.GetLocation().x << " " << e2.GetLocation().y << " " << e2.GetLocation().z << std::endl;
        // std::cout << e1.GetLocation().x << " " << e1.GetLocation().y << " " << e1.GetLocation().z << std::endl;

        if (win.QueryKey('h'))
        {
            e2.SetLocation(e2.GetLocation().x - (1 * vkan.GetDeltaT()), e2.GetLocation().y, e2.GetLocation().z);
            e1.SetLocation(e1.GetLocation().x + (1 * vkan.GetDeltaT()), e1.GetLocation().y, e1.GetLocation().z);
        }

        if (win.QueryKey('j'))
        {
            e2.SetLocation(e2.GetLocation().x + (1 * vkan.GetDeltaT()), e2.GetLocation().y, e2.GetLocation().z);
            e1.SetLocation(e1.GetLocation().x - (1 * vkan.GetDeltaT()), e1.GetLocation().y, e1.GetLocation().z);
        }

        if (win.QueryKey('w'))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y + (1 * vkan.GetDeltaT()), cam1.GetLocation().z);
        }

        if (win.QueryKey('S'))
        {
            cam1.SetLocation(cam1.GetLocation().x, cam1.GetLocation().y - (1 * vkan.GetDeltaT()), cam1.GetLocation().z);
        }

        if (win.QueryKey('A'))
        {
            cam1.SetRotation(cam1.GetRotation().x,
                             cam1.GetRotation().y,
                             cam1.GetRotation().z - (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('D'))
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
            vkan.Render(cam2);
        }
        else
        {
            vkan.Render(cam1);
        }

        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}