CXXFLAGS=-g -Wall -Weffc++ -Wstrict-null-sentinel -Wold-style-cast \
	 -Woverloaded-virtual 

all: gbcore.o

tests: tests/test_gbrom

gbcore.o: gbcore.cc gbcore.h gbrom.h
	g++ $(CXXFLAGS) -c -o $@ $<
		
tests/test_gbrom: gbrom.cc gbrom.h
	g++ -DTEST_GBROM -o $@ $<

clean:
	rm -f *.o tests/*

.PHONY: clean tests all
