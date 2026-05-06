#include "alu.h"

void ALU::compute() {
    sc_int<16> a = opA.read();
    sc_int<16> b = (is_immediate.read()) ? immediate_value.read() : opB.read(); // Mux ADD ou ADDI
    sc_int<16> c = 0;

    switch(opcode.read()) {
        case 0x0: c = a & b; break;
        case 0x1: c = a | b; break;
        case 0x2: c = a ^ b; break;
        case 0x3: c = ~a;    break;
        case 0x5: c = a + b; break; // ADD e ADDI
        case 0x6: c = a - b; break;
        default:  c = 0;
    }

    result.write(c);
    Z.write(c == 0);
    N.write(c < 0);
}