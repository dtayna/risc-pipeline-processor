#ifndef PO_H
#define PO_H

#include <systemc.h>
#include "../components/alu.h"
#include "../components/register_db.h"
#include "../components/data_memory.h"
#include "../components/instruction_memory.h"
#include "../components/program_counter.h"

SC_MODULE(PO) {
    sc_in<bool> clk;
    sc_in<bool> reset;

    const char* program_file;

    ProgramCounter*      pc;
    InstructionMemory*   imem;
    RegisterDB*          rdb;
    ALU*                 alu;
    DataMemory*          dmem;

    sc_in<bool>       ctrl_reg_write;
    sc_in<bool>       ctrl_mem_st;
    sc_in<bool>       ctrl_mem_ld;
    sc_in<bool>       ctrl_use_mem_data;
    sc_in<bool>       ctrl_is_immediate;
    sc_in<sc_uint<4>> ctrl_alu_op;
    sc_in<sc_uint<2>> ctrl_branch_type;

    sc_in<sc_uint<2>> forward_a;
    sc_in<sc_uint<2>> forward_b;
    sc_in<bool>       stall;
    sc_in<bool>       force_nop;

    sc_in<bool>        pc_enable_write;
    sc_in<bool>        pc_should_jump;
    sc_in<sc_uint<10>> pc_jump_address;


    sc_out<sc_uint<16>> instr_id_out;

    sc_out<sc_uint<4>>  id_ex_opcode_out;
    sc_out<sc_uint<4>>  id_ex_rd_out;
    sc_out<sc_uint<4>>  id_ex_rs1_out;
    sc_out<sc_uint<4>>  id_ex_rs2_out;
    sc_out<bool>        id_ex_mem_read_out;
    sc_out<sc_uint<2>>  id_ex_branch_type_out;
    sc_out<sc_uint<10>> id_ex_jump_address_out;

    sc_out<sc_uint<4>>  ex_mem_rd_out;
    sc_out<bool>        ex_mem_reg_write_out;
    sc_out<bool>        ex_mem_use_mem_data_out;

    sc_out<sc_uint<4>>  mem_wb_rd_out;
    sc_out<bool>        mem_wb_reg_write_out;

    sc_out<bool>        flag_z_out;
    sc_out<bool>        flag_n_out;

    
    sc_out<sc_int<16>> result_out; // saidinha so pra teste


    sc_signal<sc_uint<10>> pc_addr;
    sc_signal<sc_uint<16>> instr_if;

    sc_signal<sc_uint<4>>  rdb_rs1, rdb_rs2;
    sc_signal<sc_int<16>>  rdb_data_out1, rdb_data_out2;
    sc_signal<bool>        rdb_reg_write;
    sc_signal<sc_uint<4>>  rdb_rd;
    sc_signal<sc_int<16>>  rdb_data_in;

    sc_signal<sc_uint<4>>  alu_opcode;
    sc_signal<sc_int<16>>  alu_opA, alu_opB;
    sc_signal<sc_int<16>>  alu_immediate_value;
    sc_signal<bool>        alu_is_immediate;
    sc_signal<sc_int<16>>  alu_result;
    sc_signal<bool>        alu_z, alu_n;

    sc_signal<bool>        flag_z, flag_n;

    sc_signal<bool>        mem_is_st;
    sc_signal<bool>        mem_is_ld;
    sc_signal<sc_uint<8>>  mem_addr;
    sc_signal<sc_int<16>>  mem_data_st;
    sc_signal<sc_int<16>>  mem_data_ld;

    // IF/ID
    sc_signal<sc_uint<16>> IF_ID_instr;
    sc_signal<sc_uint<10>> IF_ID_pc;

    // ID/EX
    sc_signal<sc_uint<16>> ID_EX_instr;
    sc_signal<sc_uint<10>> ID_EX_pc;
    sc_signal<sc_uint<4>>  ID_EX_opcode;
    sc_signal<sc_uint<4>>  ID_EX_rd;
    sc_signal<sc_uint<4>>  ID_EX_rs1;
    sc_signal<sc_uint<4>>  ID_EX_rs2;
    sc_signal<sc_int<16>>  ID_EX_data1;
    sc_signal<sc_int<16>>  ID_EX_data2;
    sc_signal<sc_int<16>>  ID_EX_imm;
    sc_signal<bool>        ID_EX_reg_write;
    sc_signal<bool>        ID_EX_mem_st;
    sc_signal<bool>        ID_EX_mem_ld;
    sc_signal<bool>        ID_EX_use_mem_data;
    sc_signal<bool>        ID_EX_is_immediate;
    sc_signal<sc_uint<4>>  ID_EX_alu_op;
    sc_signal<sc_uint<2>>  ID_EX_branch_type;
    sc_signal<sc_uint<10>> ID_EX_jump_target;

    // EX/MEM
    sc_signal<sc_uint<4>>  EX_MEM_rd;
    sc_signal<sc_int<16>>  EX_MEM_alu_result;
    sc_signal<sc_int<16>>  EX_MEM_store_data;
    sc_signal<bool>        EX_MEM_reg_write;
    sc_signal<bool>        EX_MEM_mem_st;
    sc_signal<bool>        EX_MEM_mem_ld;
    sc_signal<bool>        EX_MEM_use_mem_data;

    // MEM/WB
    sc_signal<sc_uint<4>>  MEM_WB_rd;
    sc_signal<sc_int<16>>  MEM_WB_alu_result;
    sc_signal<sc_int<16>>  MEM_WB_mem_data;
    sc_signal<bool>        MEM_WB_reg_write;
    sc_signal<bool>        MEM_WB_use_mem_data;

    bool updates_flags(sc_uint<4> opcode) const {
        return (opcode == 0x0 || opcode == 0x1 || opcode == 0x2 || opcode == 0x3 ||
                opcode == 0x4 || opcode == 0x5 || opcode == 0x6 || opcode == 0x7);
    }

    sc_int<16> sign_extend8(sc_uint<8> imm8) const {
        sc_int<16> value = (sc_int<16>) imm8.to_uint();
        if (imm8[7] == 1) value = value | (sc_int<16>)0xFF00;
        return value;
    }

    sc_int<16> wb_value() const { // MUX vem da ALU(0) ou da memória(1)
        return MEM_WB_use_mem_data.read() ? MEM_WB_mem_data.read() : MEM_WB_alu_result.read();
    }

    sc_int<16> read_rdb_in_wb(sc_uint<4> reg_addr, sc_int<16> rdb_value) const {
        if (MEM_WB_reg_write.read() && MEM_WB_rd.read() == reg_addr) {
            return wb_value();
        }
        return rdb_value;
    }

    void ID_decode_rdb_addresses() {
        sc_uint<16> instr = IF_ID_instr.read();
        sc_uint<4> op  = instr.range(15, 12);
        sc_uint<4> rs1 = instr.range(7, 4);
        sc_uint<4> rs2 = instr.range(3, 0);

        if (op == 0x7) { // ADDI
            rdb_rs1.write(0);
            rdb_rs2.write(0);

        } else if (op == 0xA) { // LD
            rdb_rs1.write(0);
            rdb_rs2.write(0);

        } else if (op == 0xB) {  // ST
            rdb_rs1.write(0);
            rdb_rs2.write(instr.range(11, 8));

        } else {
            rdb_rs1.write(rs1);
            rdb_rs2.write(rs2);
        }
    }

    void EX_prepare_ALU_inputs() {
        sc_int<16> opA = ID_EX_data1.read();
        sc_int<16> opB = ID_EX_data2.read();

        if (forward_a.read() == 1) opA = EX_MEM_alu_result.read();
        else if (forward_a.read() == 2) opA = wb_value();

        if (forward_b.read() == 1) opB = EX_MEM_alu_result.read();
        else if (forward_b.read() == 2) opB = wb_value();

        if (ID_EX_opcode.read() == 0x7 ||
            ID_EX_opcode.read() == 0xA ||
            ID_EX_opcode.read() == 0xB) {
            opA = 0;
        }

        alu_opA.write(opA);
        alu_opB.write(opB);
        alu_immediate_value.write(ID_EX_imm.read());
        alu_is_immediate.write(ID_EX_is_immediate.read());
        alu_opcode.write(ID_EX_alu_op.read());
    }

    void MEM_connect_data_memory() {
        mem_is_st.write(EX_MEM_mem_st.read());
        mem_is_ld.write(EX_MEM_mem_ld.read());
        mem_addr.write(EX_MEM_alu_result.read().range(7, 0).to_uint());
        mem_data_st.write(EX_MEM_store_data.read());
    }

    void WB_connect_rdb_writeback() {
        rdb_reg_write.write(MEM_WB_reg_write.read());
        rdb_rd.write(MEM_WB_rd.read());
        rdb_data_in.write(wb_value());
        result_out.write(wb_value());
    }

    void update_state_to_use_in_uc() {
        instr_id_out.write(IF_ID_instr.read());

        id_ex_opcode_out.write(ID_EX_opcode.read());
        id_ex_rd_out.write(ID_EX_rd.read());
        id_ex_rs1_out.write(ID_EX_rs1.read());
        id_ex_rs2_out.write(ID_EX_rs2.read());
        id_ex_mem_read_out.write(ID_EX_mem_ld.read());
        id_ex_branch_type_out.write(ID_EX_branch_type.read());
        id_ex_jump_address_out.write(ID_EX_jump_target.read());

        ex_mem_rd_out.write(EX_MEM_rd.read());
        ex_mem_reg_write_out.write(EX_MEM_reg_write.read());
        ex_mem_use_mem_data_out.write(EX_MEM_use_mem_data.read());

        mem_wb_rd_out.write(MEM_WB_rd.read());
        mem_wb_reg_write_out.write(MEM_WB_reg_write.read());

        flag_z_out.write(flag_z.read());
        flag_n_out.write(flag_n.read());
    }

    void pipeline_registers() {
        if (reset.read()) {
            IF_ID_instr.write(0xF000);
            IF_ID_pc.write(0);

            ID_EX_instr.write(0xF000);
            ID_EX_pc.write(0);
            ID_EX_opcode.write(0xF);
            ID_EX_rd.write(0);
            ID_EX_rs1.write(0);
            ID_EX_rs2.write(0);
            ID_EX_data1.write(0);
            ID_EX_data2.write(0);
            ID_EX_imm.write(0);
            ID_EX_reg_write.write(false);
            ID_EX_mem_st.write(false);
            ID_EX_mem_ld.write(false);
            ID_EX_use_mem_data.write(false);
            ID_EX_is_immediate.write(false);
            ID_EX_alu_op.write(0);
            ID_EX_branch_type.write(0);
            ID_EX_jump_target.write(0);

            EX_MEM_rd.write(0);
            EX_MEM_alu_result.write(0);
            EX_MEM_store_data.write(0);
            EX_MEM_reg_write.write(false);
            EX_MEM_mem_st.write(false);
            EX_MEM_mem_ld.write(false);
            EX_MEM_use_mem_data.write(false);

            MEM_WB_rd.write(0);
            MEM_WB_alu_result.write(0);
            MEM_WB_mem_data.write(0);
            MEM_WB_reg_write.write(false);
            MEM_WB_use_mem_data.write(false);

            flag_z.write(false);
            flag_n.write(false);
            return;
        }

        bool do_force_nop = force_nop.read();
        bool do_stall = stall.read();

        if (updates_flags(ID_EX_opcode.read())) {
            flag_z.write(alu_z.read());
            flag_n.write(alu_n.read());
        }

        // MEM/WB
        MEM_WB_rd.write(EX_MEM_rd.read());
        MEM_WB_alu_result.write(EX_MEM_alu_result.read());
        MEM_WB_mem_data.write(mem_data_ld.read());
        MEM_WB_reg_write.write(EX_MEM_reg_write.read());
        MEM_WB_use_mem_data.write(EX_MEM_use_mem_data.read());

        // EX/MEM
        EX_MEM_rd.write(ID_EX_rd.read());
        EX_MEM_alu_result.write(alu_result.read());
        EX_MEM_store_data.write(alu_opB.read());
        EX_MEM_reg_write.write(ID_EX_reg_write.read());
        EX_MEM_mem_st.write(ID_EX_mem_st.read());
        EX_MEM_mem_ld.write(ID_EX_mem_ld.read());
        EX_MEM_use_mem_data.write(ID_EX_use_mem_data.read());

        // ID/EX
        if (do_force_nop || do_stall) {
            ID_EX_instr.write(0xF000); // NOP
            ID_EX_pc.write(0);
            ID_EX_opcode.write(0xF);
            ID_EX_rd.write(0);
            ID_EX_rs1.write(0);
            ID_EX_rs2.write(0);
            ID_EX_data1.write(0);
            ID_EX_data2.write(0);
            ID_EX_imm.write(0);
            ID_EX_reg_write.write(false);
            ID_EX_mem_st.write(false);
            ID_EX_mem_ld.write(false);
            ID_EX_use_mem_data.write(false);
            ID_EX_is_immediate.write(false);
            ID_EX_alu_op.write(0);
            ID_EX_branch_type.write(0);
            ID_EX_jump_target.write(0);
        } else {
            sc_uint<16> instr = IF_ID_instr.read();
            sc_uint<4> op  = instr.range(15, 12);
            sc_uint<4> rd  = instr.range(11, 8);
            sc_uint<4> rs1;
            sc_uint<4> rs2;

            if (op == 0x7 || op == 0xA) { // ADDI e LD 
                rs1 = 0;
                rs2 = 0;
            } else if (op == 0xB) { // ST
                rs1 = 0;
                rs2 = instr.range(11, 8);
            } else {
                rs1 = instr.range(7, 4);
                rs2 = instr.range(3, 0);
            }

            ID_EX_instr.write(instr);
            ID_EX_pc.write(IF_ID_pc.read());
            ID_EX_opcode.write(op);
            ID_EX_rd.write(rd);
            ID_EX_rs1.write(rs1);
            ID_EX_rs2.write(rs2);
            ID_EX_data1.write(read_rdb_in_wb(rs1, rdb_data_out1.read()));
            ID_EX_data2.write(read_rdb_in_wb(rs2, rdb_data_out2.read()));
            ID_EX_imm.write(sign_extend8(instr.range(7, 0)));

            ID_EX_reg_write.write(ctrl_reg_write.read());
            ID_EX_mem_st.write(ctrl_mem_st.read());
            ID_EX_mem_ld.write(ctrl_mem_ld.read());
            ID_EX_use_mem_data.write(ctrl_use_mem_data.read());
            ID_EX_is_immediate.write(ctrl_is_immediate.read());
            ID_EX_alu_op.write(ctrl_alu_op.read());
            ID_EX_branch_type.write(ctrl_branch_type.read());
            ID_EX_jump_target.write(instr.range(9, 0));
        }

        // IF/ID
        if (do_force_nop) {
            IF_ID_instr.write(0xF000);
            IF_ID_pc.write(0);
        } else if (!do_stall) {
            IF_ID_instr.write(instr_if.read());
            IF_ID_pc.write(pc_addr.read());
        }
    }

    SC_HAS_PROCESS(PO);

    PO(sc_module_name name, const char* filename) : sc_module(name), program_file(filename) {
        pc   = new ProgramCounter("pc");
        imem = new InstructionMemory("imem", program_file);
        rdb   = new RegisterDB("rdb");
        alu  = new ALU("alu");
        dmem = new DataMemory("dmem");

        pc->clk(clk);
        pc->reset(reset);
        pc->should_jump(pc_should_jump);
        pc->jump_address(pc_jump_address);
        pc->enable_write(pc_enable_write);
        pc->addr_out(pc_addr);

        imem->address(pc_addr);
        imem->instruction(instr_if);

        rdb->clk(clk);
        rdb->reg_write(rdb_reg_write);
        rdb->rs1(rdb_rs1);
        rdb->rs2(rdb_rs2);
        rdb->rd(rdb_rd);
        rdb->data_in(rdb_data_in);
        rdb->data_out1(rdb_data_out1);
        rdb->data_out2(rdb_data_out2);

        alu->opcode(alu_opcode);
        alu->opA(alu_opA);
        alu->opB(alu_opB);
        alu->immediate_value(alu_immediate_value);
        alu->is_immediate(alu_is_immediate);
        alu->result(alu_result);
        alu->Z(alu_z);
        alu->N(alu_n);

        dmem->clk(clk);
        dmem->is_st(mem_is_st);
        dmem->is_ld(mem_is_ld);
        dmem->mem_address(mem_addr);
        dmem->data_st(mem_data_st);
        dmem->data_ld(mem_data_ld);

        SC_METHOD(ID_decode_rdb_addresses);
        sensitive << IF_ID_instr;

        SC_METHOD(EX_prepare_ALU_inputs);
        sensitive << ID_EX_data1 << ID_EX_data2 << ID_EX_imm << ID_EX_is_immediate
                  << ID_EX_alu_op << ID_EX_opcode
                  << forward_a << forward_b
                  << EX_MEM_alu_result
                  << MEM_WB_use_mem_data << MEM_WB_mem_data << MEM_WB_alu_result;

        SC_METHOD(MEM_connect_data_memory);
        sensitive << EX_MEM_mem_st << EX_MEM_mem_ld << EX_MEM_alu_result << EX_MEM_store_data;

        SC_METHOD(WB_connect_rdb_writeback);
        sensitive << MEM_WB_reg_write << MEM_WB_rd << MEM_WB_use_mem_data
                  << MEM_WB_mem_data << MEM_WB_alu_result;

        SC_METHOD(update_state_to_use_in_uc);
        sensitive << IF_ID_instr
                  << ID_EX_opcode << ID_EX_rd << ID_EX_rs1 << ID_EX_rs2
                  << ID_EX_mem_ld << ID_EX_branch_type << ID_EX_jump_target
                  << EX_MEM_rd << EX_MEM_reg_write << EX_MEM_use_mem_data
                  << MEM_WB_rd << MEM_WB_reg_write << flag_z << flag_n;

        SC_METHOD(pipeline_registers);
        sensitive << clk.pos() << reset.pos();
    }

    ~PO() {
        delete pc;
        delete imem;
        delete rdb;
        delete alu;
        delete dmem;
    }
};

#endif
