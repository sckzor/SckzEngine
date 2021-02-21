CFLAGS  = -std=c++17 -O0 -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
VK_PATH = SckzCore/Vulkan
INCLUDE = SckzCore/Include.hpp

all:
	make real_all -j16

real_all: SckzEngine

SckzEngine: SckzCore/SckzEngine.cpp build/SckzCore.a SckzCore/Window/Window.hpp $(VK_PATH)/Vulkan.hpp $(INCLUDE)
	g++ $(CFLAGS) SckzCore/SckzEngine.cpp build/SckzCore.a -o SckzEngine $(LDFLAGS)

build/SckzCore.a:build/Buffer.o build/CommandBuffer.o build/Image.o build/MemoryHelper.o build/Model.o build/Pipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o $(INCLUDE)
	ar rvs build/SckzCore.a build/Buffer.o build/CommandBuffer.o build/Image.o build/MemoryHelper.o build/Model.o build/Pipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o 

build/Buffer.o: $(VK_PATH)/Buffer.cpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/MemoryHelper.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Buffer.cpp -o build/Buffer.o $(LDFLAGS)

build/CommandBuffer.o: $(VK_PATH)/CommandBuffer.cpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/CommandBuffer.cpp -o build/CommandBuffer.o $(LDFLAGS)

build/Image.o: $(VK_PATH)/Image.cpp $(VK_PATH)/Image.hpp $(VK_PATH)/MemoryHelper.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/Buffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Image.cpp -o build/Image.o $(LDFLAGS)

build/MemoryHelper.o: $(VK_PATH)/MemoryHelper.cpp $(VK_PATH)/MemoryHelper.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/MemoryHelper.cpp -o build/MemoryHelper.o $(LDFLAGS)

build/Model.o: $(VK_PATH)/Model.cpp $(VK_PATH)/Model.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Vertex.hpp $(VK_PATH)/Pipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Model.cpp -o build/Model.o $(LDFLAGS)

build/Pipeline.o: $(VK_PATH)/Pipeline.cpp $(VK_PATH)/Pipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Pipeline.cpp -o build/Pipeline.o $(LDFLAGS)

build/Vulkan.o: $(VK_PATH)/Vulkan.cpp SckzCore/Window/Window.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Model.hpp $(VK_PATH)/Pipeline.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Vulkan.cpp -o build/Vulkan.o $(LDFLAGS)

build/Window.o:SckzCore/Window/Window.cpp SckzCore/Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) -c SckzCore/Window/Window.cpp -o build/Window.o $(LDFLAGS)

build/DescriptorPool.o: $(VK_PATH)/DescriptorPool.cpp $(VK_PATH)/DescriptorPool.hpp ${INCLUDE}
	g++ $(CFLAGS) -c $(VK_PATH)/DescriptorPool.cpp -o build/DescriptorPool.o $(LDFLAGS)

test: SckzEngine
	./SckzEngine

debug: SckzEngine
	gdb SckzEngine

clean:
	rm -f build/*.o
	rm -f build/*.a
	rm -f SckzEngine