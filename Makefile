CXXFLAGS=-g -Wall -Weffc++ -Wstrict-null-sentinel -Wold-style-cast \
	 -Woverloaded-virtual 

all: gbcore.o MBC.o GBMemory.o logger.o

tests: tests/test_gbrom

logger.o: logger.cc logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

GBMemory.o: GBMemory.cc GBMemory.h
	g++ $(CXXFLAGS) -c -o $@ $<

MBC.o: MBC.cc MBC.h
	g++ $(CXXFLAGS) -c -o $@ $<

gbcore.o: gbcore.cc gbcore.h GBRom.h logger.h MBC.h GBMemory.h
	g++ $(CXXFLAGS) -c -o $@ $<
		
tests/test_gbrom: GBRom.cc GBRom.h
	g++ -DTEST_GBROM -o $@ $<

clean:
	rm -f *.o tests/*

.PHONY: clean tests all
