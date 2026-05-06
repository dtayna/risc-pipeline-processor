#include "program_counter.h"

void ProgramCounter::update_process() {
        if (reset.read()) {
            pc = 0;
        } else if (enable_write.read()) {
            if (should_jump.read()) {
                pc = jump_address.read();
            } else {
                pc = pc + 1;
            }
        }
        addr_out.write(pc);
    }