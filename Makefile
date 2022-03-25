PROGRAM   = main
CXX       = g++
CXXFLAGS  = -g -std=gnu++20 -Wall -fexceptions -lboost_system
LD_FLAGS  = -lboost_system

$(PROGRAM): main.o
	$(CXX) $(PROGRAM).cpp $(CXXFLAGS) $(LD_FLAGS)

.PHONY: clean dist

clean:
	-rm *.o

dist: clean
	-tar -chvj -C .. -f ../$(PROGRAM).tar.bz2 $(PROGRAM)


