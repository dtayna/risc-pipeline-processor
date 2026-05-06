#include <systemc.h>

SC_MODULE(ALU) {
    sc_in<sc_uint<4>>  opcode;
    sc_in<sc_int<16>>  opA, opB;
    sc_in<sc_int<16>>  immediate_value;
    sc_in<bool>        is_immediate;
    sc_out<sc_int<16>> result;
    sc_out<bool>       Z, N;

    SC_CTOR(ALU) {
        SC_METHOD(compute);
        sensitive << opcode << opA << opB << immediate_value << is_immediate;
    }

    void compute();
};