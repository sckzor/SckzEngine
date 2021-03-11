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
    sckz::Camera &           cam = vkan.CreateCamera(45, 0.1, 10);
    cam.SetLocation(0, 5, 0);
    cam.SetRotation(0, 0, 0);

    while (!win.QueryClose())
    {
        // m1.SetLocation(m1.GetLocation().x, m1.GetLocation().y + 0.002, m1.GetLocation().z);
        // m1.SetRotation(m1.GetRotation().x + 0.02, m1.GetRotation().y + 0.02, m1.GetRotation().z + 0.02);
        // m1.SetScale(m1.GetScale().x, m1.GetScale().y + 0.001, m1.GetScale().z);
        if (win.QueryKey('w'))
        {
            cam.SetLocation(cam.GetLocation().x, cam.GetLocation().y + 0.1, cam.GetLocation().z);
        }

        if (win.QueryKey('S'))
        {
            cam.SetLocation(cam.GetLocation().x, cam.GetLocation().y - 0.1, cam.GetLocation().z);
        }

        if (win.QueryKey('A'))
        {
            cam.SetRotation(cam.GetRotation().x + 0.001, cam.GetRotation().y - 1, cam.GetRotation().z);
        }

        if (win.QueryKey('D'))
        {
            cam.SetRotation(cam.GetRotation().x + 0.001, cam.GetRotation().y + 1, cam.GetRotation().z);
        }
        win.Update();
        vkan.Update(cam);
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}