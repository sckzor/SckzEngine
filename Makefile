CFLAGS  = -std=c++17 -O0 -g -DSCKZ_DEBUG
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -lopenal -lsndfile
INCLUDE = ../Include.hpp
MFLAGS = -j16

export CFLAGS LDFLAGS INCLUDE

all:
	$(MAKE) $(MFLAGS) -C SckzExtras -f SckzExtras.mk
	$(MAKE) $(MFLAGS) -C SckzSound -f SckzSound.mk
	$(MAKE) $(MFLAGS) -C SckzCore -f SckzCore.mk
	$(MAKE) $(MFLAGS) -C Shaders -f Shaders.mk
	$(MAKE) $(MFLAGS) -C Game -f Game.mk

core:
	$(MAKE) $(MFLAGS) -C SckzCore -f SckzCore.mk

extras:
	$(MAKE) $(MFLAGS) -C SckzExtras -f SckzExtras.mk

sound:
	$(MAKE) $(MFLAGS) -C SckzSound -f SckzSound.mk

game:
	$(MAKE) $(MFLAGS) -C Game -f Game.mk

shaders:
	$(MAKE) $(MFLAGS) -C Shaders -f Shaders.mk


clean:
	$(MAKE) $(MFLAGS) -C SckzCore -f SckzCore.mk clean
	$(MAKE) $(MFLAGS) -C Game -f Game.mk clean
	$(MAKE) $(MFLAGS) -C Shaders -f Shaders.mk clean
	$(MAKE) $(MFLAGS) -C SckzExtras -f SckzExtras.mk clean
	$(MAKE) $(MFLAGS) -C SckzSound -f SckzSound.mk clean


test:
	$(MAKE) all
	Game/SckzEngine

debug:
	$(MAKE) all
	gdb Game/SckzEngine

profile:
	$(MAKE) all
	mangohud Game/SckzEngine

validate:
	$(MAKE) all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt Game/SckzEngine