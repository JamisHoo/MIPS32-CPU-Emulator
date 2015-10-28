#include "cpu.h"

void CPU::instruction_decode() {
    switch (main_opcode()) {
        case 0b000000: 
            switch (sub_opcode()) {
                // sll
                case 0b000000:
                    return exe_sll();
                // srl
                case 0b000010:
                    return exe_srl();
                // sra
                case 0b000011:
                    return exe_sra();
                // sllv
                case 0b000100:
                    return exe_sllv();
                // srav
                case 0b000110:
                    return exe_srav();
                // srav
                case 0b000111:
                    return exe_srav();
                // jr
                case 0b001000:
                    return exe_jr();
                // jalr
                case 0b001001:
                    return exe_jalr();
                // syscall
                case 0b001100:
                    return exe_syscall();
                // mfhi
                case 0b010000:
                    return exe_mfhi();
                // mthi
                case 0b010001:
                    return exe_mthi();
                // mflo
                case 0b010010:
                    return exe_mflo();
                // mtlo
                case 0b010011:
                    return exe_mtlo();
                // mult
                case 0b011000:
                    return exe_mult();
                // addu
                case 0b100001:
                    return exe_addu();
                // subu
                case 0b100011:
                    return exe_subu();
                // and
                case 0b100100:
                    return exe_and();
                // or
                case 0b100101:
                    return exe_or();
                // xor
                case 0b100110:
                    return exe_xor();
                // nor
                case 0b100111:
                    return exe_nor();
                // slt
                case 0b101010:
                    return exe_slt();
                // sltu
                case 0b101011:
                    return exe_sltu();
                // TODO: exception
                default:
                    ;
            }
        case 0b000001: 
            switch (rt()) {
                // bltz
                case 0b00000:
                    return exe_bltz();
                // bgez
                case 0b00001:
                    return exe_bgez();
                // TODO: exception
                default:
                    ;
            }
        // j
        case 0b000010:
            return exe_j();
        // jal
        case 0b000011:
            return exe_jal();
        // beq
        case 0b000100:
            return exe_beq();
        // bne
        case 0b000101:
            return exe_bne();
        // blez
        case 0b000110:
            return exe_blez();
        // bgtz
        case 0b000111:
            return exe_bgtz();
        // addiu
        case 0b001001:
            return exe_addiu();
        // slti
        case 0b001010:
            return exe_slti();
        // sltiu
        case 0b001011:
            return exe_sltiu();
        // andi
        case 0b001100:
            return exe_andi();
        // ori
        case 0b001101:
            return exe_ori();
        // xori
        case 0b001110:
            return exe_xori();
        // lui
        case 0b001111:
            return exe_lui();
        case 0b010000:
            switch (sub_opcode()) {
                case 0b000000:
                    switch (rs()) {
                        // mfc0
                        case 0b00000:
                            return exe_mfc0();
                        // mtc0
                        case 0b00100:
                            return exe_mtc0();
                        // TODO: exception
                        default:
                            ; 
                    }
                // tlbwi
                case 0b000010:
                    return exe_tlbwi();
                // eret
                case 0b011000:
                    return exe_eret();
                // TODO: exception
                default:
                    ;
            }
        // lb
        case 0b100000:
            return exe_lb();
        // lw
        case 0b100011:
            return exe_lw();
        // lbu
        case 0b100100:
            return exe_lbu();
        // lhu
        case 0b100101:
            return exe_lhu();
        // sb 
        case 0b101000:
            return exe_sb();
        // sw
        case 0b101011:
            return exe_sw();
        // cache
        case 0b101111:
            return exe_cache();
            ;
    }
}
