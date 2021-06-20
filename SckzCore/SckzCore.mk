VK_PATH = Vulkan

all: SckzCore.a

# C++ based make information

SckzCore.a:build/Scene.o build/Gui.o build/ParticleSystem.o build/Buffer.o build/CommandBuffer.o build/HelperMethods.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o build/Light.o  build/Camera.o $(INCLUDE)
	ar rvs SckzCore.a build/Buffer.o build/ParticleSystem.o build/Gui.o build/Scene.o build/HelperMethods.o build/CommandBuffer.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o build/Camera.o build/Light.o

build/Buffer.o: $(VK_PATH)/Buffer.cpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Buffer.cpp -o build/Buffer.o $(LDFLAGS)

build/CommandBuffer.o: $(VK_PATH)/CommandBuffer.cpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/CommandBuffer.cpp -o build/CommandBuffer.o $(LDFLAGS)

build/Image.o: $(VK_PATH)/Image.cpp $(VK_PATH)/Image.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/Buffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Image.cpp -o build/Image.o $(LDFLAGS)

build/Memory.o: $(VK_PATH)/Memory.cpp $(VK_PATH)/Memory.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Memory.cpp -o build/Memory.o $(LDFLAGS)

build/Entity.o: $(VK_PATH)/Entity.cpp $(VK_PATH)/Entity.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Entity.cpp -o build/Entity.o $(LDFLAGS)

build/GraphicsPipeline.o: $(VK_PATH)/GraphicsPipeline.cpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/GraphicsPipeline.cpp -o build/GraphicsPipeline.o $(LDFLAGS)

build/Vulkan.o: $(VK_PATH)/Vulkan.cpp Window/Window.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/CommandBuffer.hpp Extra/Random.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Vulkan.cpp -o build/Vulkan.o $(LDFLAGS)

build/Window.o:Window/Window.cpp Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) -c Window/Window.cpp -o build/Window.o $(LDFLAGS)

build/DescriptorPool.o: $(VK_PATH)/DescriptorPool.cpp $(VK_PATH)/DescriptorPool.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/DescriptorPool.cpp -o build/DescriptorPool.o $(LDFLAGS)

build/Camera.o: $(VK_PATH)/Camera.cpp $(VK_PATH)/Camera.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Camera.cpp -o build/Camera.o $(LDFLAGS)

build/Light.o: $(VK_PATH)/Light.cpp $(VK_PATH)/Light.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Light.cpp -o build/Light.o $(LDFLAGS)

build/Scene.o: $(VK_PATH)/Scene.cpp $(VK_PATH)/Camera.hpp $(VK_PATH)/ParticleSystem.hpp $(VK_PATH)/DescriptorPool.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Model.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Scene.cpp -o build/Scene.o $(LDFLAGS)

build/HelperMethods.o: $(VK_PATH)/HelperMethods.cpp $(VK_PATH)/HelperMethods.hpp  $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/HelperMethods.cpp -o build/HelperMethods.o $(LDFLAGS)

build/Model.o: $(VK_PATH)/Model.cpp $(VK_PATH)/Model.hpp $(VK_PATH)/Entity.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/Vertex.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Model.cpp -o build/Model.o $(LDFLAGS)

build/Gui.o: $(VK_PATH)/Gui.cpp $(VK_PATH)/Gui.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Gui.cpp -o build/Gui.o $(LDFLAGS)

build/ParticleSystem.o: $(VK_PATH)/ParticleSystem.cpp $(VK_PATH)/ParticleSystem.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/ParticleSystem.cpp -o build/ParticleSystem.o $(LDFLAGS)

clean:
	rm -f build/*.o
	rm -f SckzCore.a
