#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);

    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);

    // sckz::GraphicsPipeline & p1 = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/blue_fragment.spv");
    sckz::GraphicsPipeline & p2  = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1  = vkan.CreateModel("Resources/flare.obj", "Resources/placeholder.png", p2);
    sckz::Model &            m2  = vkan.CreateModel("Resources/viking_room.obj", "Resources/viking_room.png", p2);
    sckz::Entity &           e1  = vkan.CreateEntity(m1);
    sckz::Entity &           e2  = vkan.CreateEntity(m1);
    sckz::Camera &           cam = vkan.CreateCamera(45, 0.1, 10);
    e1.SetLocation(0, 1, 0);
    cam.SetLocation(0, 5, 0);
    cam.SetRotation(-90, 0, 0);

    win.Update();
    vkan.Update(cam);
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
            cam.SetLocation(cam.GetLocation().x, cam.GetLocation().y + (1 * vkan.GetDeltaT()), cam.GetLocation().z);
        }

        if (win.QueryKey('S'))
        {
            cam.SetLocation(cam.GetLocation().x, cam.GetLocation().y - (1 * vkan.GetDeltaT()), cam.GetLocation().z);
        }

        if (win.QueryKey('A'))
        {
            cam.SetRotation(cam.GetRotation().x, cam.GetRotation().y, cam.GetRotation().z - (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('D'))
        {
            cam.SetRotation(cam.GetRotation().x, cam.GetRotation().y, cam.GetRotation().z + (10 * vkan.GetDeltaT()));
        }

        if (win.QueryKey('q'))
        {
            cam.SetRotation(cam.GetRotation().x - (10 * vkan.GetDeltaT()), cam.GetRotation().y, cam.GetRotation().z);
        }

        if (win.QueryKey('e'))
        {
            cam.SetRotation(cam.GetRotation().x + (10 * vkan.GetDeltaT()), cam.GetRotation().y, cam.GetRotation().z);
        }
        win.Update();
        vkan.Update(cam);
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}