#include <systemc.h>

SC_MODULE(DataMemory) {
    sc_in<bool>          clk;
    sc_in<bool>          is_st;
    sc_in<bool>          is_ld;
    sc_in<sc_uint<8>>    mem_address;
    sc_in<sc_int<16>>    data_st;
    sc_out<sc_int<16>>   data_ld;

    sc_int<16> data_memory[256];

    SC_CTOR(DataMemory) {
        SC_METHOD(write_process);
        sensitive << clk.pos();

        SC_METHOD(read_process);
        sensitive << is_ld << mem_address;

        for (int i = 0; i < 256; i++) data_memory[i] = 0;
    }

    void write_process();
    void read_process();

};