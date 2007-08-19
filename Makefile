CXXFLAGS=-g -Wall -Weffc++ -Wstrict-null-sentinel -Wold-style-cast \
	 -Woverloaded-virtual 

all: gbcore.o MBC.o GBMemory.o Logger.o GBRom.o

tests: tests/test_gbrom

Logger.o: Logger.cc Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

GBMemory.o: GBMemory.cc GBMemory.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

MBC.o: MBC.cc MBC.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

gbcore.o: gbcore.cc gbcore.h GBRom.h Logger.h MBC.h GBMemory.h
	g++ $(CXXFLAGS) -c -o $@ $<
		
tests/test_gbrom: GBRom.cc GBRom.h Logger.o
	g++ -DTEST_GBROM -o $@ GBRom.cc Logger.o 

clean:
	rm -f *.o tests/test_gbrom

.PHONY: clean tests all
