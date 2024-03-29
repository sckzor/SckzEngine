VK_PATH = Vulkan

all: SckzCore.a

# C++ based make information

SckzCore.a:build/Scene.o build/CubeMap.o build/Gui.o build/Timer.o build/ParticleSystem.o build/Buffer.o build/Combine.o build/CommandBuffer.o build/Filter.o build/HelperMethods.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o build/Light.o  build/Camera.o build/Fbo.o build/Bone.o build/Animation.o build/Animator.o $(INCLUDE)
	ar rvs SckzCore.a build/CubeMap.o build/Buffer.o build/Timer.o build/ParticleSystem.o build/Gui.o build/Combine.o build/Scene.o build/Filter.o build/HelperMethods.o build/CommandBuffer.o build/Image.o build/Memory.o build/Model.o build/Entity.o build/GraphicsPipeline.o build/Vulkan.o build/DescriptorPool.o build/Window.o build/Camera.o build/Light.o build/Fbo.o build/Bone.o build/Animation.o build/Animator.o

build/Buffer.o: $(VK_PATH)/Buffer.cpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Buffer.cpp -o build/Buffer.o $(LDFLAGS)

build/CommandBuffer.o: $(VK_PATH)/CommandBuffer.cpp $(VK_PATH)/CommandBuffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/CommandBuffer.cpp -o build/CommandBuffer.o $(LDFLAGS)

build/Image.o: $(VK_PATH)/Image.cpp $(VK_PATH)/Image.hpp $(VK_PATH)/Memory.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/Buffer.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Image.cpp -o build/Image.o $(LDFLAGS)

build/Memory.o: $(VK_PATH)/Memory.cpp $(VK_PATH)/Memory.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Memory.cpp -o build/Memory.o $(LDFLAGS)

build/Entity.o: ../SckzExtras/LightSort.hpp $(VK_PATH)/Entity.cpp $(VK_PATH)/Entity.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/ $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Entity.cpp -o build/Entity.o $(LDFLAGS)

build/GraphicsPipeline.o: $(VK_PATH)/GraphicsPipeline.cpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Fbo.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/GraphicsPipeline.cpp -o build/GraphicsPipeline.o $(LDFLAGS)

build/Vulkan.o: $(VK_PATH)/Vulkan.cpp Window/Window.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/CommandBuffer.hpp Extra/Random.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Vulkan.cpp -o build/Vulkan.o $(LDFLAGS)

build/Window.o:Window/Window.cpp Window/Window.hpp $(INCLUDE)
	g++ $(CFLAGS) -c Window/Window.cpp -o build/Window.o $(LDFLAGS)

build/DescriptorPool.o: $(VK_PATH)/DescriptorPool.cpp $(VK_PATH)/DescriptorPool.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/DescriptorPool.cpp -o build/DescriptorPool.o $(LDFLAGS)

build/Camera.o: $(VK_PATH)/Camera.cpp $(VK_PATH)/Camera.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Camera.cpp -o build/Camera.o $(LDFLAGS)

build/Light.o: $(VK_PATH)/Light.cpp $(VK_PATH)/Light.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Light.cpp -o build/Light.o $(LDFLAGS)

build/Scene.o: $(VK_PATH)/Scene.cpp $(VK_PATH)/CubeMap.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/ParticleSystem.hpp $(VK_PATH)/DescriptorPool.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Model.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Scene.cpp -o build/Scene.o $(LDFLAGS)

build/HelperMethods.o: $(VK_PATH)/HelperMethods.cpp $(VK_PATH)/HelperMethods.hpp  $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/HelperMethods.cpp -o build/HelperMethods.o $(LDFLAGS)

build/Model.o: $(VK_PATH)/Model.cpp $(VK_PATH)/Model.hpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/Entity.hpp $(VK_PATH)/BoneInfo.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Camera.hpp $(VK_PATH)/Vertex.hpp $(VK_PATH)/Bone.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Model.cpp -o build/Model.o $(LDFLAGS)

build/Gui.o: $(VK_PATH)/Gui.cpp $(VK_PATH)/Gui.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Gui.cpp -o build/Gui.o $(LDFLAGS)

build/ParticleSystem.o: $(VK_PATH)/ParticleSystem.cpp $(VK_PATH)/ParticleSystem.hpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/Buffer.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/GraphicsPipeline.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/ParticleSystem.cpp -o build/ParticleSystem.o $(LDFLAGS)

build/Fbo.o: $(VK_PATH)/Fbo.cpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/DescriptorPool.hpp $(VK_PATH)/HelperMethods.hpp $(VK_PATH)/Image.hpp
	g++ $(CFLAGS) -c $(VK_PATH)/Fbo.cpp -o build/Fbo.o $(LDFLAGS)

build/Filter.o: $(VK_PATH)/Filter.cpp $(VK_PATH)/Filter.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Memory.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Filter.cpp -o build/Filter.o $(LDFLAGS)

build/Combine.o: $(VK_PATH)/Combine.cpp $(VK_PATH)/Combine.hpp $(VK_PATH)/CommandBuffer.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Memory.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Combine.cpp -o build/Combine.o $(LDFLAGS)

build/CubeMap.o: $(VK_PATH)/CubeMap.cpp $(VK_PATH)/CubeMap.hpp $(VK_PATH)/GraphicsPipeline.hpp $(VK_PATH)/Image.hpp $(VK_PATH)/Scene.hpp $(VK_PATH)/Vertex.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/CubeMap.cpp -o build/CubeMap.o $(LDFLAGS)

build/Timer.o: $(VK_PATH)/Timer.cpp $(VK_PATH)/Timer.hpp $(VK_PATH)/Vulkan.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Timer.cpp -o build/Timer.o $(LDFLAGS)

build/Bone.o: $(VK_PATH)/Bone.cpp $(VK_PATH)/Bone.hpp $(VK_PATH)/BoneInfo.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Bone.cpp -o build/Bone.o $(LDFLAGS)

build/Animation.o: $(VK_PATH)/Animation.cpp $(VK_PATH)/Animation.hpp $(VK_PATH)/Bone.hpp $(VK_PATH)/BoneInfo.hpp $(VK_PATH)/Model.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Animation.cpp -o build/Animation.o $(LDFLAGS)

build/Animator.o: $(VK_PATH)/Animator.cpp $(VK_PATH)/Animator.hpp $(VK_PATH)/Animation.hpp $(INCLUDE)
	g++ $(CFLAGS) -c $(VK_PATH)/Animator.cpp -o build/Animator.o $(LDFLAGS)

clean:
	rm -f build/*.o
	rm -f SckzCore.a
