#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc.h>

SC_MODULE(RegisterDB) {
    sc_in<bool>          clk;
    sc_in<bool>          reg_write;
    sc_in<sc_uint<4>>    rs1, rs2, rd;
    sc_in<sc_int<16>>    data_in;
    sc_out<sc_int<16>>   data_out1, data_out2;

    sc_int<16> registers[16];

    SC_CTOR(RegisterDB) {
        SC_METHOD(read_process);
        sensitive << rs1 << rs2 << clk.pos();
        dont_initialize();


        SC_METHOD(write_process);
        sensitive << clk.pos();
        dont_initialize();

        for (int i = 0; i < 16; i++) registers[i] = 0;
    }

    void read_process();
    void write_process();
};

#endif
