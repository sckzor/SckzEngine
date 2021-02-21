#include "SckzEngine.hpp"

int main() {
    sckz::Window win;
    win.CreateWindow("SckzEngine", 800, 600);
    sckz::Vulkan vkan;
    vkan.CreateVulkan(win);
    vkan.CreatePipeline("vertex.spv", "fragment.spv");
    sckz::Model m1 = vkan.CreateModel("flare.obj", "placeholder.png");
    sckz::Model m2 = vkan.CreateModel("viking_room.obj", "placeholder.png");
    sckz::Model m3 = vkan.CreateModel("untitled.obj", "placeholder.png");
    
    while(!win.QueryClose()) {
        win.Update();
        vkan.Update();
    }

    vkan.DestroyVulkan();
    win.DestroyWindow();

    return 0;
}