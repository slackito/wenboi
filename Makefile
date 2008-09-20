#CXXFLAGS=-pg -O3 -g -Wall -Weffc++ -Wstrict-null-sentinel -Wold-style-cast 
CXXFLAGS=-pg -g -Wall -Weffc++ -Wold-style-cast \
	 -Woverloaded-virtual $(shell sdl-config --cflags)
LDFLAGS=-pg -g $(shell sdl-config --libs)

all: tests

tests: tests/test_gbrom tests/test_core wendi/wendi

util.o: util.cc util.h
	g++ $(CXXFLAGS) -c -o $@ $<

GBVideo.o: GBVideo.cc GBVideo.h Logger.h util.h gbcore.h
	g++ $(CXXFLAGS) -c -o $@ $<

GBMemory.o: GBMemory.cc GBMemory.h Logger.h MBC.h gbcore.h
	g++ $(CXXFLAGS) -c -o $@ $<

MBC.o: MBC.cc MBC.h Logger.h NoMBC.h MBC1.h
	g++ $(CXXFLAGS) -c -o $@ $<

NoMBC.o: NoMBC.cc NoMBC.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

MBC1.o: MBC1.cc MBC1.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

gbcore.o: gbcore.cc gbcore.h opcodes.h \
	GBRom.h Logger.h MBC.h GBMemory.h GBVideo.h util.h
	g++ $(CXXFLAGS) -c -o $@ $<
		
tests/test_gbrom: GBRom.cc GBRom.h 
	g++ $(CXXFLAGS)  -DTEST_GBROM -o $@ GBRom.cc $(LDFLAGS)

tests/test_core: tests/test_core.cc gbcore.o MBC.o GBMemory.o GBRom.o \
	GBVideo.o util.o NoMBC.o MBC1.o wendi/disasm.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

wendi/CodeBlock.o: wendi/CodeBlock.cc wendi/CodeBlock.h
	g++ $(CXXFLAGS) -c -o $@ $<

wendi/disasm.o: wendi/disasm.cc wendi/disasm.h wendi/Instruction.h
	g++ $(CXXFLAGS) -c -o $@ $<

wendi/output_txt.o: wendi/output_txt.cc wendi/output_txt.h wendi/disassembly_output.h
	g++ $(CXXFLAGS) -c -o $@ $<

wendi/output_graph.o: wendi/output_graph.cc wendi/output_graph.h wendi/disassembly_output.h Logger.h
	g++ $(CXXFLAGS) -c -o $@ $<

wendi/wendi: wendi/wendi.cc wendi/CodeBlock.o wendi/disasm.o \
	wendi/output_txt.o wendi/output_graph.o gbcore.o MBC.o \
	GBMemory.o GBRom.o GBVideo.o util.o NoMBC.o MBC1.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o wendi/*.o wendi/wendi tests/test_gbrom tests/test_core

.PHONY: clean tests all
