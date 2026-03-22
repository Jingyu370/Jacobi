export I_MPI_CXX ?= icx
CXX      := mpiicpc

ROWS     ?= 128
COLS     ?= 128
NP       ?= 4

CXXFLAGS := -std=c++17 -Wall -Wextra -g -I./inc \
            -DTOTAL_ROWS=$(ROWS) \
            -DTOTAL_COLS=$(COLS)
            
LDFLAGS  := -lstdc++

SRCDIR   := src
OBJDIR   := obj
SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS  := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
OUTPUT   := jacobi_2d

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(OUTPUT)
	@mkdir -p result
	mpirun -np $(NP) ./$(OUTPUT)

clean:
	rm -rf $(OUTPUT) $(OBJDIR)

.PHONY: run clean