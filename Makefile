CXX := g++
CXXFLAGS := -O3 -march=native -mavx2 -std=c++17 -Wall -Wextra -DNDEBUG

BIN := bench
SRCDIR := src
SCRIPTDIR := scripts

SRC_COMMON := $(SRCDIR)/bench.cpp \
              $(SRCDIR)/generator.cpp \
              $(SRCDIR)/case_converter_SIMD.cpp

OBJ_COMMON := $(SRC_COMMON:.cpp=.o)
OBJ_SERIAL := $(SRCDIR)/case_converter_serial.o

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ_COMMON) $(OBJ_SERIAL)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilación normal
%.o: %.cpp $(SRCDIR)/common.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilar el serial con vectorización desactivada
$(SRCDIR)/case_converter_serial.o: $(SRCDIR)/case_converter_serial.cpp $(SRCDIR)/common.hpp
	$(CXX) $(CXXFLAGS) -fno-tree-vectorize -c $< -o $@

run: all
	./$(BIN) -a 20
	python3 $(SCRIPTDIR)/plot_results.py results_aligned.csv results_misaligned.csv

clean:
	rm -f $(BIN) $(SRCDIR)/*.o results_*.csv figura2.png
