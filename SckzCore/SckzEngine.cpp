#include "SckzEngine.hpp"

int main()
{
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);
    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);
    vkan.CreatePipeline("Resources/vertex.spv", "Resources/fragment.spv");
    sckz::Model m1 = vkan.CreateModel("Resources/flare.obj", "Resources/placeholder.png");
    sckz::Model m2 = vkan.CreateModel("Resources/viking_room.obj", "Resources/placeholder.png");
    sckz::Model m3 = vkan.CreateModel("Resources/untitled.obj", "Resources/placeholder.png");

    while (!win.QueryClose())
    {
        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}