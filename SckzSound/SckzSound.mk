
all: SckzSound.a

SckzSound.a:  build/SoundBuffer.o build/MusicBuffer.o build/SoundDevice.o build/SoundSource.o
	ar rvs SckzSound.a build/SoundBuffer.o build/MusicBuffer.o build/SoundDevice.o build/SoundSource.o

build/SoundBuffer.o: SoundBuffer.cpp SoundBuffer.hpp
	g++ $(CFLAGS) -c SoundBuffer.cpp -o build/SoundBuffer.o $(LDFLAGS)

build/MusicBuffer.o: MusicBuffer.cpp MusicBuffer.hpp
	g++ $(CFLAGS) -c MusicBuffer.cpp -o build/MusicBuffer.o $(LDFLAGS)

build/SoundDevice.o: SoundDevice.cpp SoundDevice.hpp
	g++ $(CFLAGS) -c SoundDevice.cpp -o build/SoundDevice.o $(LDFLAGS)

build/SoundSource.o: SoundSource.cpp SoundSource.hpp
	g++ $(CFLAGS) -c SoundSource.cpp -o build/SoundSource.o $(LDFLAGS)

clean:
	rm -f build/*.o
	rm -f SckzSound.a
