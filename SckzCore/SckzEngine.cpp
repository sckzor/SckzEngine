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
    sckz::Model &            m2 = vkan.CreateModel("Resources/viking_room.obj", "Resources/placeholder.png", p2);
    // sckz::Model &            m3 = vkan.CreateModel("Resources/untitled.obj", "Resources/placeholder.png", p1);

    while (!win.QueryClose())
    {
        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}