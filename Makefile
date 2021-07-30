CXXFLAGS= -ggdb -Wall -O0

DEPS=build/RIFFFile.o build/RIFFSubChunk.o build/util.o

riffedit: main.cpp $(wildcard *.h) $(wildcard *.cpp) $(DEPS)
	$(CXX) main.cpp $(wildcard build/*.o) -o build/riffedit $(CXXFLAGS)
	rm -f ./riffedit
	ln -s build/riffedit ./riffedit

build/RIFFFile.o: RIFFFile.h RIFFFile.cpp
	$(CXX) RIFFFile.cpp $(CXXFLAGS) -c -o build/RIFFFile.o

build/RIFFSubChunk.o: RIFFSubChunk.h RIFFSubChunk.cpp util.h
	$(CXX) RIFFSubChunk.cpp $(CXXFLAGS) -c -o build/RIFFSubChunk.o

build/util.o: util.h util.cpp
	$(CXX) util.cpp $(CXXFLAGS) -c -o build/util.o

clean:
	rm -rf build/
	mkdir build
	rm -f ./riffedit

.PHONY: clean
