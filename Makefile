CXX = g++
CXXFLAGS = -I/usr/include -lsystemc -O2 -std=c++17 -fPIC


COMPONENTS = components/alu.cpp components/register_db.cpp components/program_counter.cpp components/data_memory.cpp
TARGET = processador_sim
SRCS = $(COMPONENTS) src/main.cpp

ASSEMBLER_BIN = ./assembler
ASSEMBLER_SRC = src/pre_proccess.cpp
ASM_INPUT = tests/alg5_stall.txt # -----
BIN_OUTPUT = programa.bin

all: $(TARGET)


$(ASSEMBLER_BIN): $(ASSEMBLER_SRC)
	$(CXX) $(ASSEMBLER_SRC) -o $(ASSEMBLER_BIN)


$(BIN_OUTPUT): $(ASM_INPUT) $(ASSEMBLER_BIN)
	$(ASSEMBLER_BIN) $(ASM_INPUT) $(BIN_OUTPUT)

$(TARGET): $(SRCS) $(BIN_OUTPUT)
	$(CXX) $(SRCS) $(CXXFLAGS) -o $(TARGET)


run: all
	LD_LIBRARY_PATH=/usr/lib ./$(TARGET)

clean:
	rm -f $(TARGET) $(ASSEMBLER_BIN) $(BIN_OUTPUT) bin_test_* *.vcd
