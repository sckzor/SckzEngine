VULKAN = ../SckzCore/Vulkan

Tracy.o: 
	g++ $(CFLAGS) TracyClient.cpp -o Tracy.o $(LDFLAGS)

SckzEngine: SckzEngine.cpp ../SckzCore/SckzCore.a $(VULKAN)/Scene.hpp $(VULKAN)/Vulkan.hpp $(VULKAN)/Model.hpp Tracy.o $(VULKAN)/Fbo.hpp $(VULKAN)/Entity.hpp ../SckzCore/Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) SckzEngine.cpp ../SckzCore/SckzCore.a Tracy.o -o SckzEngine $(LDFLAGS)

clean:
	rm -f SckzEngine
	