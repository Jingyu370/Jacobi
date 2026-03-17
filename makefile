export I_MPI_CXX ?= icx

CXX		    := mpiicpc
CXXFLAGS 	:= -std=c++17 -Wall -Wextra -g
LDFLAGS		:= -lstdc++

NAME		:= $(shell basename $(CURDIR))
SOURCES		:= $(wildcard *.cpp)
OBJECTS		:= $(SOURCES:.cpp=.o)
OUTPUT		:= $(NAME)

TOTALSIZE	?= 128
NP			?= 4

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -DTOTALSIZE=$(TOTALSIZE) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -DTOTALSIZE=$(TOTALSIZE) -c $< -o $@

run:
	mpirun -np $(NP) ./$(OUTPUT)

clean:
	rm -rf $(OUTPUT) *.o

.PHONY: run clean