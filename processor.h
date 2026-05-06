#include <systemc.h>
#include "po.h"
#include "uc.h"

SC_MODULE(Processor) {
    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<sc_int<16>> result_out;

    PO *po;
    UC *uc;

    // UC -> PO
    sc_signal<bool>       ctrl_reg_write;
    sc_signal<bool>       ctrl_mem_st;
    sc_signal<bool>       ctrl_mem_ld;
    sc_signal<bool>       ctrl_use_mem_data;
    sc_signal<bool>       ctrl_is_immediate;
    sc_signal<sc_uint<4>> ctrl_alu_op;
    sc_signal<sc_uint<2>> ctrl_branch_type; // 00 = sem, 01 = J, 10 = JZ, 11 = JN

    sc_signal<sc_uint<2>> forward_a;
    sc_signal<sc_uint<2>> forward_b;
    sc_signal<bool>       stall;
    sc_signal<bool>       force_nop;

    sc_signal<bool>        pc_enable_write;
    sc_signal<bool>        pc_should_jump;
    sc_signal<sc_uint<10>> pc_jump_address;

    // PO -> UC
    sc_signal<sc_uint<16>> instr_id;

    sc_signal<sc_uint<4>>  id_ex_opcode;
    sc_signal<sc_uint<4>>  id_ex_rd;
    sc_signal<sc_uint<4>>  id_ex_rs1;
    sc_signal<sc_uint<4>>  id_ex_rs2;
    sc_signal<bool>        id_ex_mem_read;
    sc_signal<sc_uint<2>>  id_ex_branch_type;
    sc_signal<sc_uint<10>> id_ex_jump_address;

    sc_signal<sc_uint<4>>  ex_mem_rd;
    sc_signal<bool>        ex_mem_reg_write;
    sc_signal<bool>        ex_mem_use_mem_data;

    sc_signal<sc_uint<4>>  mem_wb_rd;
    sc_signal<bool>        mem_wb_reg_write;

    sc_signal<bool>        flag_z;
    sc_signal<bool>        flag_n;

    SC_HAS_PROCESS(Processor);

    Processor(sc_module_name name, const char* program_file) : sc_module(name) {
        po = new PO("ParteOperativa", program_file);
        uc = new UC("UnidadeControle");

        // Conexoes

        po->clk(clk);
        po->reset(reset);
        po->result_out(result_out);

        // Entradas PO
        po->ctrl_reg_write(ctrl_reg_write);
        po->ctrl_mem_st(ctrl_mem_st);
        po->ctrl_mem_ld(ctrl_mem_ld);
        po->ctrl_use_mem_data(ctrl_use_mem_data);
        po->ctrl_is_immediate(ctrl_is_immediate);
        po->ctrl_alu_op(ctrl_alu_op);
        po->ctrl_branch_type(ctrl_branch_type);

        po->forward_a(forward_a);
        po->forward_b(forward_b);
        po->stall(stall);
        po->force_nop(force_nop);

        po->pc_enable_write(pc_enable_write);
        po->pc_should_jump(pc_should_jump);
        po->pc_jump_address(pc_jump_address);

        // Saidas PO
        po->instr_id_out(instr_id);
        po->id_ex_opcode_out(id_ex_opcode);
        po->id_ex_rd_out(id_ex_rd);
        po->id_ex_rs1_out(id_ex_rs1);
        po->id_ex_rs2_out(id_ex_rs2);
        po->id_ex_mem_read_out(id_ex_mem_read);
        po->id_ex_branch_type_out(id_ex_branch_type);
        po->id_ex_jump_address_out(id_ex_jump_address);

        po->ex_mem_rd_out(ex_mem_rd);
        po->ex_mem_reg_write_out(ex_mem_reg_write);
        po->ex_mem_use_mem_data_out(ex_mem_use_mem_data);

        po->mem_wb_rd_out(mem_wb_rd);
        po->mem_wb_reg_write_out(mem_wb_reg_write);

        po->flag_z_out(flag_z);
        po->flag_n_out(flag_n);

        // Entradas UC
        uc->instr_id(instr_id);
        uc->id_ex_opcode(id_ex_opcode);
        uc->id_ex_rd(id_ex_rd);
        uc->id_ex_rs1(id_ex_rs1);
        uc->id_ex_rs2(id_ex_rs2);
        uc->id_ex_mem_ld(id_ex_mem_read);
        uc->id_ex_branch_type(id_ex_branch_type);
        uc->id_ex_jump_address(id_ex_jump_address);

        uc->ex_mem_rd(ex_mem_rd);
        uc->ex_mem_reg_write(ex_mem_reg_write);
        uc->ex_mem_use_mem_data(ex_mem_use_mem_data);

        uc->mem_wb_rd(mem_wb_rd);
        uc->mem_wb_reg_write(mem_wb_reg_write);

        uc->flag_z(flag_z);
        uc->flag_n(flag_n);

        // Saídas UC
        uc->ctrl_reg_write(ctrl_reg_write);
        uc->ctrl_mem_st(ctrl_mem_st);
        uc->ctrl_mem_ld(ctrl_mem_ld);
        uc->ctrl_use_mem_data(ctrl_use_mem_data);
        uc->ctrl_is_immediate(ctrl_is_immediate);
        uc->ctrl_alu_op(ctrl_alu_op);
        uc->ctrl_branch_type(ctrl_branch_type);

        uc->forward_a(forward_a);
        uc->forward_b(forward_b);
        uc->stall(stall);
        uc->force_nop(force_nop);

        uc->pc_enable_write(pc_enable_write);
        uc->pc_should_jump(pc_should_jump);
        uc->pc_jump_address(pc_jump_address);
    }

    ~Processor() {
        delete po;
        delete uc;
    }
};

