CXX = ccache g++
override CXXFLAGS += -Wall -g -ggdb -pthread -std=c++11
SRC = $(wildcard *.cpp) 
OBJ = $(patsubst %.cpp,%.o,$(SRC))

all: $(OBJ)
	$(CXX) $(CXXFLAGS) -o mydns $(OBJ)

clean:
	-rm -rf $(OBJ) mydns
