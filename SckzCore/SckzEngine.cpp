#include "SckzEngine.hpp"

int main() {
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);
    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);
    vkan.CreatePipeline("vertex.spv", "fragment.spv");
    vkan.CreateModel("flare.obj", "viking_room.obj", "placeholder.png");
    
    while(!win.QueryClose()) {
        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}