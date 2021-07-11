CXXFLAGS= -ggdb -Wall

riffedit: main.cpp $(wildcard *.h) $(wildcard *.cpp)
	$(CXX) main.cpp $(wildcard build/*.o) -o build/riffedit $(CXXFLAGS)
	rm ./riffedit
	ln -s build/riffedit ./riffedit

