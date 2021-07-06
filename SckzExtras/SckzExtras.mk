VK_PATH = ../SckzCore/Vulkan

all: SckzExtras.a

SckzExtras.a: build/Bloom.o build/LightSort.o
	ar rvs SckzExtras.a build/Bloom.o build/LightSort.o

build/Bloom.o: Bloom.cpp Bloom.hpp $(VK_PATH)/Combine.hpp $(VK_PATH)/Fbo.hpp $(VK_PATH)/Filter.hpp $(VK_PATH)/Scene.hpp $(INCLUDE)
	g++ $(CFLAGS) -c Bloom.cpp -o build/Bloom.o $(LDFLAGS)

build/LightSort.o: LightSort.cpp LightSort.hpp $(VK_PATH)/Light.hpp $(INCLUDE)
	g++ $(CFLAGS) -c LightSort.cpp -o build/LightSort.o $(LDFLAGS)


clean:
	rm -f build/*.o
	rm -f SckzExtras.a