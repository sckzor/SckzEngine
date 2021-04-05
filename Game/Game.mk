SckzEngine: SckzEngine.cpp ../SckzCore/SckzCore.a $(INCLUDE)
	g++ $(CFLAGS) SckzEngine.cpp ../SckzCore/SckzCore.a -o SckzEngine $(LDFLAGS)

clean:
	rm -f SckzEngine