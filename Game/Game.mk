VULKAN = ../SckzCore/Vulkan

SckzEngine: SckzEngine.cpp ../SckzCore/SckzCore.a $(VULKAN)/Scene.hpp $(VULKAN)/Vulkan.hpp $(VULKAN)/Model.hpp $(VULKAN)/Entity.hpp ../SckzCore/Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) SckzEngine.cpp ../SckzCore/SckzCore.a -o SckzEngine $(LDFLAGS)

clean:
	rm -f SckzEngine
	