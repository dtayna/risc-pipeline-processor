#ifndef UC_H
#define UC_H

#include <systemc.h>

SC_MODULE(UC) {

    sc_in<sc_uint<16>> instr_id;

    sc_in<sc_uint<4>>  id_ex_opcode;
    sc_in<sc_uint<4>>  id_ex_rd;
    sc_in<sc_uint<4>>  id_ex_rs1;
    sc_in<sc_uint<4>>  id_ex_rs2;
    sc_in<bool>        id_ex_mem_ld;
    sc_in<sc_uint<2>>  id_ex_branch_type;
    sc_in<sc_uint<10>> id_ex_jump_address;

    sc_in<sc_uint<4>>  ex_mem_rd;
    sc_in<bool>        ex_mem_reg_write;
    sc_in<bool>        ex_mem_use_mem_data;

    sc_in<sc_uint<4>>  mem_wb_rd;
    sc_in<bool>        mem_wb_reg_write;

    sc_in<bool>        flag_z;
    sc_in<bool>        flag_n;


    sc_out<bool>       ctrl_reg_write;
    sc_out<bool>       ctrl_mem_st;
    sc_out<bool>       ctrl_mem_ld;
    sc_out<bool>       ctrl_use_mem_data;
    sc_out<bool>       ctrl_is_immediate;
    sc_out<sc_uint<4>> ctrl_alu_op;
    sc_out<sc_uint<2>> ctrl_branch_type;

    sc_out<sc_uint<2>> forward_a;
    sc_out<sc_uint<2>> forward_b;
    sc_out<bool>       stall;
    sc_out<bool>       force_nop;

    sc_out<bool>       pc_enable_write;
    sc_out<bool>       pc_should_jump;
    sc_out<sc_uint<10>> pc_jump_address;


    void gerar_controle() {
        sc_uint<16> instr = instr_id.read();
        sc_uint<4>  op    = instr.range(15, 12);
        sc_uint<4>  rs1   = (op == 0x7) ? (sc_uint<4>)0 : (sc_uint<4>)instr.range(7, 4); // ADDI fica R0 como rs1
        sc_uint<4>  rs2   = instr.range(3, 0);

        // Decoficador
        bool reg_write = false;
        bool mem_st = false;
        bool mem_ld = false;
        bool use_mem_data = false;
        bool is_immediate = false;
        sc_uint<4> alu_op = op;
        sc_uint<2> branch_type = 0;

        switch (op) {
            case 0x0: // AND
            case 0x1: // OR
            case 0x2: // XOR
            case 0x3: // NOT
            case 0x5: // ADD
            case 0x6: // SUB
                reg_write = true;
                break;

            case 0x4: // CMP (só atualizei flags)
                break;

            case 0x7: // ADDI
                reg_write = true;
                is_immediate = true;
                alu_op = 0x5;
                break;

            case 0xA: // LD
                reg_write = true;
                mem_ld = true;
                use_mem_data = true;
                is_immediate = true;
                alu_op = 0x5;
                break;

            case 0xB: // ST
                mem_st = true;
                is_immediate = true;
                alu_op = 0x5;
                break;

            case 0xC: // J
                branch_type = 1;
                break;

            case 0xD: // JZ
                branch_type = 2;
                break;

            case 0xE: // JN
                branch_type = 3;
                break;

            case 0xF: // NOP
            default:
                break;
        }

        ctrl_reg_write.write(reg_write);
        ctrl_mem_st.write(mem_st);
        ctrl_mem_ld.write(mem_ld);
        ctrl_use_mem_data.write(use_mem_data);
        ctrl_is_immediate.write(is_immediate);
        ctrl_alu_op.write(alu_op);
        ctrl_branch_type.write(branch_type);

        // load-use stall
        bool load_use = false;
        if (id_ex_mem_ld.read()) { 
            if (id_ex_rd.read() == rs1) load_use = true;
            if (id_ex_rd.read() == rs2) load_use = true;
        }

        stall.write(load_use);
        pc_enable_write.write(!load_use); // parar o pc

        // fowarding
        sc_uint<2> fa = 00;
        sc_uint<2> fb = 00;

        if (ex_mem_reg_write.read() && !ex_mem_use_mem_data.read() &&
            ex_mem_rd.read() != 0 && ex_mem_rd.read() == id_ex_rs1.read()) {
            fa = 1;
        } else if (mem_wb_reg_write.read() &&
                   mem_wb_rd.read() != 0 && mem_wb_rd.read() == id_ex_rs1.read()) {
            fa = 2;
        }

        if (ex_mem_reg_write.read() && !ex_mem_use_mem_data.read() &&
            ex_mem_rd.read() != 0 && ex_mem_rd.read() == id_ex_rs2.read()) {
            fb = 1;
        } else if (mem_wb_reg_write.read() &&
                   mem_wb_rd.read() != 0 && mem_wb_rd.read() == id_ex_rs2.read()) {
            fb = 2;
        }

        forward_a.write(fa);
        forward_b.write(fb);


        // Branch/jump hazard
        bool should_jump = false;
        if (id_ex_branch_type.read() == 1) should_jump  = true;
        if (id_ex_branch_type.read() == 2 && flag_z.read()) should_jump  = true;
        if (id_ex_branch_type.read() == 3 && flag_n.read()) should_jump  = true;

        pc_should_jump.write(should_jump );
        pc_jump_address.write(id_ex_jump_address.read());
        force_nop.write(should_jump);
    }

    SC_CTOR(UC) {
        SC_METHOD(gerar_controle);
        sensitive << instr_id
                  << id_ex_opcode << id_ex_rd << id_ex_rs1 << id_ex_rs2
                  << id_ex_mem_ld << id_ex_branch_type << id_ex_jump_address
                  << ex_mem_rd << ex_mem_reg_write << ex_mem_use_mem_data
                  << mem_wb_rd << mem_wb_reg_write
                  << flag_z << flag_n;
    }
};

#endif
