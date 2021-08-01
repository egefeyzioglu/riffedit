CXXFLAGS= -ggdb -Wall -O0

BUILDDIR=build

# grab all cpp files with their path that are in some DIRS list
SRC = $(wildcard *.cpp) $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.cpp))

# stick the .obj/ directory before the .cpp file and change the extension
OBJ = $(addprefix $(BUILDDIR)/,$(SRC:.cpp=.o))

riffedit: $(OBJ)
	g++ -o $(BUILDDIR)/riffedit $(BUILDDIR)/*.o
	rm -f ./riffedit
	ln -s $(BUILDDIR)/riffedit ./riffedit

build/main.o: main.cpp
	$(COMPILE.cpp) -o $@ $<

$(BUILDDIR)/%.o : %.cpp %.h
	@mkdir -p $(@D)
	$(COMPILE.cpp) -o $@ $<

clean:
	rm -rf build/
	mkdir build
	rm -f ./riffedit

.PHONY: clean
