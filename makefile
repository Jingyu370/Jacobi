export I_MPI_CXX ?= icx

CXX		    := mpiicpc
CXXFLAGS 	:= -std=c++17 -Wall -Wextra -g
LDFLAGS		:= -lstdc++

NAME		:= $(shell basename $(CURDIR))
SOURCES		:= $(wildcard *.cpp)
OBJECTS		:= $(SOURCES:.cpp=.o)
OUTPUT		:= $(NAME)

NP			?= 4

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	mpirun -np $(NP) ./$(OUTPUT)

clean:
	rm -rf $(OUTPUT) *.o

.PHONY: run clean