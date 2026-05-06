#include <systemc.h>
#include <iostream>
#include "../core/processor.h"

int sc_main(int argc, char* argv[]) {
    const char* program_file = "programa.bin";

    if (argc >= 2) {
        program_file = argv[1];
    }

    sc_clock clk("clk", 10, SC_NS);
    sc_signal<bool> reset;
    sc_signal<sc_int<16>> final_res;

    Processor cpu("Processador", program_file);
    cpu.clk(clk);
    cpu.reset(reset);
    cpu.result_out(final_res);

    sc_trace_file *fp = sc_create_vcd_trace_file("simulacao");
    fp->set_time_unit(1, SC_NS);

    // Sinais externos
    sc_trace(fp, clk, "clock");
    // sc_trace(fp, reset, "reset");
    sc_trace(fp, final_res, "resultado_final");

    // IF
    // sc_trace(fp, cpu.po->pc_addr, "IF_pc_addr");
    sc_trace(fp, cpu.po->instr_if, "IF_instr");
    sc_trace(fp, cpu.po->pc_enable_write, "IF_pc_enable");
    // sc_trace(fp, cpu.po->pc_should_jump, "IF_pc_load_branch");
    // sc_trace(fp, cpu.po->pc_jump_address, "IF_pc_target");

    // IF/ID
    sc_trace(fp, cpu.po->IF_ID_instr, "IF_ID_instr");
    // sc_trace(fp, cpu.po->IF_ID_pc, "IF_ID_pc");

    // ID/EX
    sc_trace(fp, cpu.po->ID_EX_instr, "ID_EX_instr");
    // sc_trace(fp, cpu.po->ID_EX_opcode, "ID_EX_opcode");
    // sc_trace(fp, cpu.po->ID_EX_rd, "ID_EX_rd");
    // sc_trace(fp, cpu.po->ID_EX_rs1, "ID_EX_rs1");
    // sc_trace(fp, cpu.po->ID_EX_rs2, "ID_EX_rs2");
    // sc_trace(fp, cpu.po->ID_EX_data1, "ID_EX_data1");
    // sc_trace(fp, cpu.po->ID_EX_data2, "ID_EX_data2");
    // sc_trace(fp, cpu.po->ID_EX_imm, "ID_EX_imm");
    // sc_trace(fp, cpu.po->ID_EX_reg_write, "ID_EX_reg_write");
    // sc_trace(fp, cpu.po->ID_EX_mem_ld, "ID_EX_mem_ld");
    // sc_trace(fp, cpu.po->ID_EX_mem_st, "ID_EX_mem_st");
    // sc_trace(fp, cpu.po->ID_EX_is_immediate, "ID_EX_is_immediate");
    // sc_trace(fp, cpu.po->ID_EX_jump_target, "ID_EX_jump_target");

    // EX / ALU
    // sc_trace(fp, cpu.po->alu_opcode, "EX_alu_opcode");
    // sc_trace(fp, cpu.po->alu_opA, "EX_alu_opA");
    // sc_trace(fp, cpu.po->alu_opB, "EX_alu_opB");
    // sc_trace(fp, cpu.po->alu_immediate_value, "EX_alu_imm");
    // sc_trace(fp, cpu.po->alu_is_immediate, "EX_alu_select_imm");
    sc_trace(fp, cpu.po->alu_result, "EX_alu_result");
    // sc_trace(fp, cpu.po->alu_z, "EX_alu_z");
    // sc_trace(fp, cpu.po->alu_n, "EX_alu_n");

    // EX/MEM
    // sc_trace(fp, cpu.po->EX_MEM_rd, "EX_MEM_rd");
    sc_trace(fp, cpu.po->EX_MEM_alu_result, "EX_MEM_alu_result");
    sc_trace(fp, cpu.po->EX_MEM_store_data, "EX_MEM_store_data");
    sc_trace(fp, cpu.po->EX_MEM_reg_write, "EX_MEM_reg_write");
    // sc_trace(fp, cpu.po->EX_MEM_mem_ld, "EX_MEM_mem_ld");
    // sc_trace(fp, cpu.po->EX_MEM_mem_st, "EX_MEM_mem_st");

    // MEM/WB
    //sc_trace(fp, cpu.po->MEM_WB_rd, "MEM_WB_rd");
    sc_trace(fp, cpu.po->MEM_WB_alu_result, "MEM_WB_alu_result");
    // sc_trace(fp, cpu.po->MEM_WB_mem_data, "MEM_WB_mem_data");
    sc_trace(fp, cpu.po->MEM_WB_reg_write, "MEM_WB_reg_write");
    // sc_trace(fp, cpu.po->MEM_WB_use_mem_data, "MEM_WB_use_mem_data");

    // Hazard/flush
    // sc_trace(fp, cpu.po->stall, "hazard_stall");
    // sc_trace(fp, cpu.po->force_nop, "control_force_nop");

    // Write-back
    sc_trace(fp, cpu.po->rdb_reg_write, "rdb_reg_write");
    sc_trace(fp, cpu.po->rdb_rd, "rdb_rd");
    sc_trace(fp, cpu.po->rdb_data_in, "rdb_data_in");


    // sc_trace(fp, cpu.po->rdb->registers[0], "R0");
    sc_trace(fp, cpu.po->rdb->registers[1], "R1");
    sc_trace(fp, cpu.po->rdb->registers[2], "R2");
    sc_trace(fp, cpu.po->rdb->registers[3], "R3");
    sc_trace(fp, cpu.po->rdb->registers[4], "R4");
    sc_trace(fp, cpu.po->rdb->registers[5], "R5");
    sc_trace(fp, cpu.po->rdb->registers[6], "R6");
    sc_trace(fp, cpu.po->rdb->registers[7], "R7");
    // sc_trace(fp, cpu.po->rdb->registers[8], "R8");
    // sc_trace(fp, cpu.po->rdb->registers[9], "R9");
    // sc_trace(fp, cpu.po->rdb->registers[10], "R10");
    // sc_trace(fp, cpu.po->rdb->registers[11], "R11");
    // sc_trace(fp, cpu.po->rdb->registers[12], "R12");
    // sc_trace(fp, cpu.po->rdb->registers[13], "R13");
    // sc_trace(fp, cpu.po->rdb->registers[14], "R14");
    // sc_trace(fp, cpu.po->rdb->registers[15], "R15");

    // sc_trace(fp, cpu.po->pc_addr, "pc_addr");
    sc_trace(fp, cpu.po->pc_addr,   "pc_addr");
    sc_trace(fp, cpu.po->pc_should_jump,   "pc_load_branch_jump");
    sc_trace(fp, cpu.po->pc_jump_address, "pc_target");
    sc_trace(fp, cpu.po->force_nop,     "force_nop");
    sc_trace(fp, cpu.po->stall,     "stall");
    sc_trace(fp, cpu.po->forward_a,     "forward_a");
    sc_trace(fp, cpu.po->forward_b,     "forward_b");
    // sc_trace(fp, cpu.po->EX_MEM_mem_st, "ex_MEM_mem_st");

    reset.write(true);
    sc_start(20, SC_NS);
    reset.write(false);

    sc_start(300, SC_NS);
    

    std::cout << "Simulacao concluida." << std::endl;

    sc_close_vcd_trace_file(fp);
    return 0;
}
