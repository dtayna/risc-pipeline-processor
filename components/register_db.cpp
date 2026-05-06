#include "register_db.h"

void RegisterDB::read_process() {
    data_out1.write(registers[rs1.read()]);
    data_out2.write(registers[rs2.read()]);
}

void RegisterDB::write_process() {
    if (reg_write.read()) {
        sc_uint<4> dest = rd.read();

        if (dest != 0) { // R0 sempre 0
            registers[dest] = data_in.read();
        }
    }
}
