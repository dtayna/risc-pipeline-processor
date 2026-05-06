#include <systemc.h>

SC_MODULE(InstructionMemory) {
    sc_in<sc_uint<10>> address;
    sc_out<sc_uint<16>> instruction;
    
    sc_uint<16> rom[1024];

    InstructionMemory(sc_module_name name, const char* filename) : sc_module(name) {
        SC_METHOD(read_process);
        sensitive << address;

        std::ifstream file(filename);
        unsigned int val;
        int i = 0;

        while (file >> std::hex >> val && i < 1024) {
            rom[i++] = val;
        }
    }

    void read_process() {
        instruction.write(rom[address.read()]);
    }
};