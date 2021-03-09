#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);
    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);
    // sckz::GraphicsPipeline & p1 = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/blue_fragment.spv");
    sckz::GraphicsPipeline & p2 = vkan.CreatePipeline("Resources/simple_vertex.spv", "Resources/simple_fragment.spv");
    sckz::Model &            m1 = vkan.CreateModel("Resources/flare.obj", "Resources/placeholder.png", p2);
    sckz::Model &            m2 = vkan.CreateModel("Resources/viking_room.obj", "Resources/viking_room.png", p2);
    // sckz::Model &            m3 = vkan.CreateModel("Resources/untitled.obj", "Resources/placeholder.png", p1);

    while (!win.QueryClose())
    {
        m1.SetLocation(m1.GetLocation().x, m1.GetLocation().y + 0.002, m1.GetLocation().z);
        m1.SetRotation(m1.GetRotation().x + 0.02, m1.GetRotation().y + 0.02, m1.GetRotation().z + 0.02);
        m1.SetScale(m1.GetScale().x, m1.GetScale().y + 0.001, m1.GetScale().z);
        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}