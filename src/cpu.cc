#include "cpu.h"

void CPU::instruction_decode(bool& exception) {
    switch (main_opcode()) {
        case 0b000000: 
            switch (sub_opcode()) {
                // sll
                case 0b000000:
                    return exe_sll(exception);
                // srl
                case 0b000010:
                    return exe_srl(exception);
                // sra
                case 0b000011:
                    return exe_sra(exception);
                // sllv
                case 0b000100:
                    return exe_sllv(exception);
                // srav
                case 0b000110:
                    return exe_srlv(exception);
                // srav
                case 0b000111:
                    return exe_srav(exception);
                // jr
                case 0b001000:
                    return exe_jr(exception);
                // jalr
                case 0b001001:
                    return exe_jalr(exception);
                // syscall
                case 0b001100:
                    return exe_syscall(exception);
                // mfhi
                case 0b010000:
                    return exe_mfhi(exception);
                // mthi
                case 0b010001:
                    return exe_mthi(exception);
                // mflo
                case 0b010010:
                    return exe_mflo(exception);
                // mtlo
                case 0b010011:
                    return exe_mtlo(exception);
                // mult
                case 0b011000:
                    return exe_mult(exception);
                // addu
                case 0b100001:
                    return exe_addu(exception);
                // subu
                case 0b100011:
                    return exe_subu(exception);
                // and
                case 0b100100:
                    return exe_and(exception);
                // or
                case 0b100101:
                    return exe_or(exception);
                // xor
                case 0b100110:
                    return exe_xor(exception);
                // nor
                case 0b100111:
                    return exe_nor(exception);
                // slt
                case 0b101010:
                    return exe_slt(exception);
                // sltu
                case 0b101011:
                    return exe_sltu(exception);
                default:
                    cp0_.set_exception_code(cp0_.Exc_RI);
                    exception = true;
                    return;
            }
        case 0b000001: 
            switch (rt()) {
                // bltz
                case 0b00000:
                    return exe_bltz(exception);
                // bgez
                case 0b00001:
                    return exe_bgez(exception);
                default:
                    cp0_.set_exception_code(cp0_.Exc_RI);
                    exception = true;
                    return;
            }
        // j
        case 0b000010:
            return exe_j(exception);
        // jal
        case 0b000011:
            return exe_jal(exception);
        // beq
        case 0b000100:
            return exe_beq(exception);
        // bne
        case 0b000101:
            return exe_bne(exception);
        // blez
        case 0b000110:
            return exe_blez(exception);
        // bgtz
        case 0b000111:
            return exe_bgtz(exception);
        // addiu
        case 0b001001:
            return exe_addiu(exception);
        // slti
        case 0b001010:
            return exe_slti(exception);
        // sltiu
        case 0b001011:
            return exe_sltiu(exception);
        // andi
        case 0b001100:
            return exe_andi(exception);
        // ori
        case 0b001101:
            return exe_ori(exception);
        // xori
        case 0b001110:
            return exe_xori(exception);
        // lui
        case 0b001111:
            return exe_lui(exception);
        case 0b010000:
            switch (sub_opcode()) {
                case 0b000000:
                    switch (rs()) {
                        // mfc0
                        case 0b00000:
                            return exe_mfc0(exception);
                        // mtc0
                        case 0b00100:
                            return exe_mtc0(exception);
                        default:
                            cp0_.set_exception_code(cp0_.Exc_RI);
                            exception = true;
                            return;
                    }
                // tlbwi
                case 0b000010:
                    return exe_tlbwi(exception);
                // eret
                case 0b011000:
                    return exe_eret(exception);
                default:
                    cp0_.set_exception_code(cp0_.Exc_RI);
                    exception = true;
                    return;
            }
        // lb
        case 0b100000:
            return exe_lb(exception);
        // lw
        case 0b100011:
            return exe_lw(exception);
        // lbu
        case 0b100100:
            return exe_lbu(exception);
        // lhu
        case 0b100101:
            return exe_lhu(exception);
        // sb 
        case 0b101000:
            return exe_sb(exception);
        // sw
        case 0b101011:
            return exe_sw(exception);
        // cache
        case 0b101111:
            return exe_cache(exception);
        default:
            cp0_.set_exception_code(cp0_.Exc_RI);
            exception = true;
            return;
    }
}
