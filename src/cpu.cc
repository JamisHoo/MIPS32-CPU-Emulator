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
    registers_[rd()] = registers_[rs()] + registers_[rd()];
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

void CPU::exe_slt() {
    registers_[rd()] = int32_t(registers_[rs()]) < int32_t(registers_[rt()])? 0x01, 0x00;
    pc_ += 4;
}

void CPU::exe_sltu() {
    registers_[rd()] = uint32_t(registers_[rs()]) < uint32_t(registers_[rt()])? 0x01, 0x00;
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
    pc_ = pc_ & 0xf0000000 | jump_target() * 4;
}

void CPU::exe_jal() {
    registers_[REG_RA] = pc_ + 4;
    pc_ = pc_ & 0xf0000000 | jump_target() * 4;
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
    mmu_.tlb_data_[0][cp0_.registers_[cp0_.Index]] = cp0_registers_[cp0_.EntryLo0];
    mmu_.tlb_data_[1][cp0_.registers_[cp0_.Index]] = cp0_registers_[cp0_.EntryLo1];
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

void CPU::instruction_decode() {
    switch (main_opcode(instruction)) {
        case 0b000000: 
            switch (sub_opcode(instruction)) {
                // sll
                case 0b000000:
                // srl
                case 0b000010:
                // sra
                case 0b000011:
                // sllv
                case 0b000100:
                // srlv
                case 0b000110:
                // srav
                case 0b000111:
                // jr
                case 0b001000:
                // jalr
                case 0b001001:
                // syscall
                case 0b001100:
                // mfhi
                case 0b010000:
                // mthi
                case 0b010001:
                // mflo
                case 0b010010:
                // mtlo
                case 0b010011:
                // mult
                case 0b011000:
                // addu
                case 0b100001:
                // subu
                case 0b100011:
                // and
                case 0b100100:
                // or
                case 0b100101:
                // xor
                case 0b100110:
                // nor
                case 0b100111:
                // slt
                case 0b101010:
                // sltu
                case 0b101011:
                // TODO: exception
                default:
            }
        case 0b000001: 
            switch (rt(instruction)) {
                // bltz
                case 0b00000:
                // bgez
                case 0b00001:
                // TODO: exception
                default:
            }
        // j
        case 0b000010:
        // jal
        case 0b000011:
        // beq
        case 0b000100:
        // bne
        case 0b000101:
        // blez
        case 0b000110:
        // bgtz
        case 0b000111:
        // addiu
        case 0b001001:
        // slti
        case 0b001010:
        // sltiu
        case 0b001011:
        // andi
        case 0b001100:
        // ori
        case 0b001101:
        // xori
        case 0b001110:
        // lui
        case 0b001111:
        case 0b010000:
            switch (sub_opcode(instruction)) {
                case 0b000000:
                    switch (rs(instruction)) {
                        // mfc0
                        case 0b00000:
                        // mtc0
                        case 0b00100:
                        // TODO: exception
                        default:
                            
                    }
                // tlbwi
                case 0b000010:
                // eret
                case 0b011000:
                // TODO: exception
                default:
            }
        // lb
        case 0b100000:
        // lw
        case 0b100011:
        // lbu
        case 0b100100:
        // lhu
        case 0b100101:
        // sb 
        case 0b101000:
        // sw
        case 0b101011:
        // cache
        case 0b101111:
    }
}
