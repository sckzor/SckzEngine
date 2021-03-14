CFLAGS  = -std=c++17 -O0 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
VK_PATH = SckzCore/Vulkan
INCLUDE = SckzCore/Include.hpp

all:
	make real_all -j16

real_all: SckzEngine Shaders

# C++ based make information

SckzEngine: SckzCore/SckzEngine.cpp build/SckzCore.a SckzCore/Window/Window.hpp $(VK_PATH)/Vulkan.hpp $(INCLUDE)
	g++ $(CFLAGS) SckzCore/SckzEngine.cpp build/SckzCore.a -o SckzEngine $(LDFLAGS)

build/SckzCore.a:build/Buffer.o build/CommandBuffer.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o  build/Camera.o $(INCLUDE)
	ar rvs build/SckzCore.a build/Buffer.o build/CommandBuffer.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o build/Camera.o

build/Buffer.o: $(VK_PATH)/Buffer.cpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Buffer.cpp -o build/Buffer.o $(LDFLAGS)

build/CommandBuffer.o: $(VK_PATH)/CommandBuffer.cpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/CommandBuffer.cpp -o build/CommandBuffer.o $(LDFLAGS)

build/Image.o: $(VK_PATH)/Image.cpp $(VK_PATH)/Image.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/Buffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Image.cpp -o build/Image.o $(LDFLAGS)

build/Memory.o: $(VK_PATH)/Memory.cpp $(VK_PATH)/Memory.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Memory.cpp -o build/Memory.o $(LDFLAGS)

build/Model.o: $(VK_PATH)/Model.cpp $(VK_PATH)/Model.hpp $(VK_PATH)/Entity.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/Vertex.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Model.cpp -o build/Model.o $(LDFLAGS)

build/Entity.o: $(VK_PATH)/Entity.cpp $(VK_PATH)/Entity.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Entity.cpp -o build/Entity.o $(LDFLAGS)

build/GraphicsPipeline.o: $(VK_PATH)/GraphicsPipeline.cpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/GraphicsPipeline.cpp -o build/GraphicsPipeline.o $(LDFLAGS)

build/Vulkan.o: $(VK_PATH)/Vulkan.cpp SckzCore/Window/Window.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Model.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Vulkan.cpp -o build/Vulkan.o $(LDFLAGS)

build/Window.o:SckzCore/Window/Window.cpp SckzCore/Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) -c SckzCore/Window/Window.cpp -o build/Window.o $(LDFLAGS)

build/DescriptorPool.o: $(VK_PATH)/DescriptorPool.cpp $(VK_PATH)/DescriptorPool.hpp ${INCLUDE}
	g++ $(CFLAGS) -c $(VK_PATH)/DescriptorPool.cpp -o build/DescriptorPool.o $(LDFLAGS)

build/Camera.o: $(VK_PATH)/Camera.cpp $(VK_PATH)/Camera.hpp ${INCLUDE}
	g++ $(CFLAGS) -c $(VK_PATH)/Camera.cpp -o build/Camera.o $(LDFLAGS)

# GLSL based make information

Shaders:SimpleShaders BlueShaders

SimpleShaders: Resources/simple_vertex.spv Resources/simple_fragment.spv

Resources/simple_fragment.spv: Shaders/simple_shader.frag
	glslc Shaders/simple_shader.frag -o Resources/simple_fragment.spv

Resources/simple_vertex.spv: Shaders/simple_shader.vert
	glslc Shaders/simple_shader.vert -o Resources/simple_vertex.spv


BlueShaders: Resources/blue_fragment.spv

Resources/blue_fragment.spv: Shaders/blue_shader.frag
	glslc Shaders/blue_shader.frag -o Resources/blue_fragment.spv


# Testing based information

test: SckzEngine
	make all
	./SckzEngine

debug: SckzEngine
	make all
	gdb SckzEngine

clean:
	rm -f build/*.o
	rm -f build/*.a
	rm -f SckzEngine