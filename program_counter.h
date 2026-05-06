#include <systemc.h>

SC_MODULE(ProgramCounter) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_in<bool> should_jump;
    sc_in<bool> enable_write;
    sc_in<sc_uint<10>> jump_address;
    sc_out<sc_uint<10>> addr_out;

    sc_uint<10> pc;

    SC_CTOR(ProgramCounter) {
        SC_METHOD(update_process);
        sensitive << clk.pos() << reset.pos();

        pc = 0;
    }

    void update_process();
};