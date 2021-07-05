VULKAN = ../SckzCore/Vulkan

SckzEngine: SckzEngine.cpp ../SckzExtras/SckzExtras.a ../SckzCore/SckzCore.a $(VULKAN)/Scene.hpp $(VULKAN)/Vulkan.hpp $(VULKAN)/Model.hpp $(VULKAN)/Fbo.hpp $(VULKAN)/Entity.hpp ../SckzCore/Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) SckzEngine.cpp ../SckzCore/SckzCore.a ../SckzExtras/SckzExtras.a -o SckzEngine $(LDFLAGS)

clean:
	rm -f SckzEngine
	