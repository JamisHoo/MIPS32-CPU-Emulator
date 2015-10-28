#include "cpu.h"

void CPU::exe_sll() {
    registers_[rd()] = registers_[rt()] << shift();
    pc_ += 4;
}

void CPU::exe_srl() {
    registers_[rd()] = registers_[rt()] >> shift();
    pc_ += 4;
}

void CPU::exe_sra() {
    int32_t val = registers_[rt()];
    val >>= shift();
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_sllv() {
    registers_[rd()] = registers_[rt()] >> registers_[rs()];
    pc_ += 4;
}

void CPU::exe_srav() {
    int32_t val = registers_[rt()];
    val >>= registers_[rs()];
    registers_[rd()] = val;
    pc_ += 4;
}

void CPU::exe_jr() {
    pc_ = registers_[rs()];
}

void CPU::exe_jalr() {
    registers_[rd()] = pc_ + 4;
    pc_ = registers_[rs()];
}

void CPU::exe_syscall() {
    // TODO: syscall
}

void CPU::exe_mfhi() {
    registers_[rd()] = hi_;
    pc_ += 4;
}

void CPU::exe_mthi() {
    hi_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_mflo() {
    registers_[rd()] = lo_;
    pc_ += 4;
}

void CPU::exe_mtlo() {
    lo_ = registers_[rs()];
    pc_ += 4;
}

void CPU::exe_mult() {
    int64_t a = registers_[rs()];
    int64_t b = registers_[rt()];
    a *= b;
    hi_ = uint32_t(a >> 32);
    lo_ = uint32_t(a);
    pc_ += 4;
}

void CPU::exe_addu() {
    registers_[rd()] = registers_[rs()] + registers_[rt()];
    pc_ += 4;
}

void CPU::exe_subu() {
    registers_[rd()] = registers_[rs()] - registers_[rt()];
    pc_ += 4;
}

void CPU::exe_and() {
    registers_[rd()] = registers_[rs()] & registers_[rt()];
    pc_ += 4;
}

void CPU::exe_or() {
    registers_[rd()] = registers_[rs()] | registers_[rt()];
    pc_ += 4;
}

void CPU::exe_xor() {
    registers_[rd()] = registers_[rs()] ^ registers_[rt()];
    pc_ += 4;
}

void CPU::exe_nor() {
    registers_[rd()] = ~(registers_[rs()] | registers_[rt()]);
    pc_ += 4;
}

void CPU::exe_slt() {
    registers_[rd()] = int32_t(registers_[rs()]) < int32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltu() {
    registers_[rd()] = uint32_t(registers_[rs()]) < uint32_t(registers_[rt()])? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_bltz() {
    if (int32_t(registers_[rs()]) < 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bgez() {
    if (int32_t(registers_[rs()]) >= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_j() {
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
}

void CPU::exe_jal() {
    registers_[REG_RA] = pc_ + 4;
    pc_ = (pc_ & 0xf0000000) | jump_target() * 4;
}

void CPU::exe_beq() {
    if (registers_[rs()] == registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bne() {
    if (registers_[rs()] != registers_[rt()])
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_blez() {
    if (int32_t(registers_[rs()]) <= 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_bgtz() {
    if (int32_t(registers_[rs()]) > 0)
        pc_ += branch_offset() * 4;
    pc_ += 4;
}

void CPU::exe_addiu() {
    registers_[rt()] = registers_[rs()] + signed_immediate();
    pc_ += 4;
}

void CPU::exe_slti() {
    registers_[rt()] = int32_t(registers_[rs()]) < signed_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_sltiu() {
    registers_[rt()] = registers_[rs()] < unsigned_immediate()? 0x01: 0x00;
    pc_ += 4;
}

void CPU::exe_andi() {
    registers_[rt()] = registers_[rs()] & unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_ori() {
    registers_[rt()] = registers_[rs()] | unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_xori() {
    registers_[rt()] = registers_[rs()] ^ unsigned_immediate();
    pc_ += 4;
}

void CPU::exe_lui() {
    registers_[rt()] = uint32_t(unsigned_immediate()) << 16;
    pc_ += 4;
}

void CPU::exe_mfc0() {
    registers_[rt()] = cp0_.registers_[rd()];
    pc_ += 4;
}

void CPU::exe_mtc0() {
    cp0_.registers_[rd()] = registers_[rt()];
    pc_ += 4;
}

void CPU::exe_tlbwi() {
    mmu_.tlb_key_[cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryHi];
    mmu_.tlb_data_[0][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo0];
    mmu_.tlb_data_[1][cp0_.registers_[cp0_.Index]] = cp0_.registers_[cp0_.EntryLo1];
    pc_ += 4;
}

void CPU::exe_eret() {
    pc_ = cp0_.registers_[cp0_.EPC];
    // TODO: who should be responsible for restoring EXL?
    //cp0_.registers_[cp0_.SR] &= 0xfffffffd;
}

void CPU::exe_lb() {
    int8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate());
    registers_[rt()] = int32_t(val); 
    pc_ += 4;
}

void CPU::exe_lw() {
    uint32_t val = mmu_.read_word(registers_[rs()] + signed_immediate());
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_lbu() {
    uint8_t val = mmu_.read_byte(registers_[rs()] + signed_immediate());
    registers_[rt()] = val; 
    pc_ += 4;
}

void CPU::exe_lhu() {
    uint16_t val = mmu_.read_half_word(registers_[rs()] + signed_immediate());
    registers_[rt()] = val;
    pc_ += 4;
}

void CPU::exe_sb() {
    mmu_.write_byte(registers_[rs()] + signed_immediate(), registers_[rt()] & 0xff);
    pc_ += 4;
}

void CPU::exe_sw() {
    mmu_.write_word(registers_[rs()] + signed_immediate(), registers_[rt()]);
    pc_ += 4;
}

void CPU::exe_cache() {
    pc_ += 4;
}
