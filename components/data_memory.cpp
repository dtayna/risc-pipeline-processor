#include "data_memory.h"

void DataMemory::write_process() {
        if (is_st.read()) {
            data_memory[mem_address.read()] = data_st.read();
        }
    }

void DataMemory::read_process() {
        if (is_ld.read()) {
            data_ld.write(data_memory[mem_address.read()]);
        } else {
            data_ld.write(0);
        }
    }