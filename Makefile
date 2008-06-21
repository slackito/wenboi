CXXFLAGS=-g -Wall -Weffc++ -Wstrict-null-sentinel -Wold-style-cast \
	 -Woverloaded-virtual 
LDFLAGS=-g

all: tests

tests: tests/test_gbrom tests/test_core

GBVideo.o: GBVideo.cc GBVideo.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

GBMemory.o: GBMemory.cc GBMemory.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

MBC.o: MBC.cc MBC.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

gbcore.o: gbcore.cc gbcore.h GBRom.h Logger.h MBC.h GBMemory.h
	g++ $(CXXFLAGS) -c -o $@ $<
		
tests/test_gbrom: GBRom.cc GBRom.h 
	g++ -DTEST_GBROM -o $@ GBRom.cc 

tests/test_core: tests/test_core.cc gbcore.o MBC.o GBMemory.o GBRom.o GBVideo.o
	g++ -o $@ $^

clean:
	rm -f *.o tests/test_gbrom

.PHONY: clean tests all
