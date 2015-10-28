#include "cp0.h"
#include "mmu.h"

class CPU {
public:
    CPU(std::ifstream& rom, std::ifstream& flash): mmu_(rom, flash), pc_(PC_INITIAL) { }

    void run() {
        while (true) 
            next();
    }
    
private:
    void next() {
        registers_[REG_ZERO] = 0x00;

        cp0_.registers_[cp0_.Count]++;
        if (cp0_.registers_[cp0_.Count] == cp0_.registers[cp0_.Compare]) {
            // TODO: timer interrupt
            // TODO: who is responsible for clearing Count register
            // TODO: should init Count and Compare register?
        }

        // instruction fetch
        instruction_ = mmu_.read_word(pc_);

        // instruction decode
        instruction_decode();
    }

    void instruction_decode();

    void exe_sll(); 
    void exe_srl(); 
    void exe_sra(); 
    void exe_sllv(); 
    void exe_srav(); 
    void exe_jr();
    void exe_jalr();
    void exe_syscall();
    void exe_mfhi();
    void exe_mthi();
    void exe_mflo();
    void exe_mtlo();
    void exe_mult();
    void exe_addu();
    void exe_subu();
    void exe_and();
    void exe_or();
    void exe_xor();
    void exe_nor();
    void exe_slt();
    void exe_sltu();
    void exe_bltz();
    void exe_bgez();
    void exe_j();
    void exe_jal();
    void exe_beq();
    void exe_bne();
    void exe_blez();
    void exe_bgtz();
    void exe_addiu();
    void exe_slti();
    void exe_sltiu();
    void exe_andi();
    void exe_ori();
    void exe_xori();
    void exe_lui();
    void exe_mfc0();
    void exe_mtc0();
    void exe_tlbwi();
    void exe_eret();
    void exe_lb();
    void exe_lw();
    void exe_lbu();
    void exe_lhu();
    void exe_sb();
    void exe_sw();
    void exe_cache();


    uint32_t main_opcode() const { return instruction_ >> 26; }
    uint32_t sub_opcode() const { return instruction_ & 0x3f; }
    uint32_t rs() const { return instruction_ >> 21 & 0x1f; }
    uint32_t rt() const { return instruction_ >> 16 & 0x1f; }
    uint32_t rd() const { return instruction_ >> 11 & 0x1f; }
    uint32_t shift() const { return instruction_ >> 6 & 0x1f; }
    int16_t branch_offset() const { return int16_t(instruction_ & 0xffff); }
    uint32_t jump_target() const { return instruction_ & 0x3ffffff; }
    int16_t signed_immediate() const { return int16_t(instruction_ & 0xffff); }
    uint16_t unsigned_immediate() const { return uint16_t(instruction_ & 0xffff); }

    constexpr static uint32_t PC_INITIAL = 0xbfc00000;

    CP0 cp0_;
    MMU mmu_;

    enum { 
        REG_ZERO = 0, 
        REG_AT = 1, 
        REG_V0 = 2, REG_V1, 
        REG_A0 = 4, REG_A1, REG_A2, REG_A3,
        REG_T0 = 8, REG_T1, REG_T2, REG_T3, REG_T4, REG_T5, REG_T6, REG_T7, 
        REG_T8 = 24, REG_T9, 
        REG_S0 = 16, REG_S1, REG_S2, REG_S3, REG_S4, REG_S5, REG_S6, REG_S7,
        REG_K0 = 26, REG_K1, 
        REG_GP = 28,
        REG_SP = 29, 
        REG_S8 = 30, REG_FP = 30,
        REG_RA = 31
    };

    uint32_t registers_[32];
    uint32_t pc_;
    uint32_t instruction_;
    uint32_t hi_, lo_;

};
