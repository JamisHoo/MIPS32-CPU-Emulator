#ifndef CPU_
#define CPU_
#include "cp0.h"
#include "mmu.h"

class CPU {
public:
    CPU(std::ifstream& rom, std::ifstream& flash): cp0_(), mmu_(cp0_, rom, flash), pc_(PC_INITIAL) { }

    void run() {
        while (true) {
            bool exception = false;
            next(exception);
            
            if (exception) {

                // store return pc only in normal level
                if (!cp0_.Status_EXL())
                    cp0_.registers_[cp0_.EPC] = pc_;
                // jump to EBase
                pc_ = cp0_.registers_[cp0_.EBase];
                // set EXL to 1
                cp0_.set_Status_EXL();
            }
        }
    }
    
private:
    void next(bool& exception) {

        registers_[REG_ZERO] = 0x00;

        cp0_.registers_[cp0_.Count]++;

        
        // TODO: set BadVAddr when address-related exception occurs
        // TODO: who is responsible for clearing Count register
        // TODO: does KSU count?
        // TODO: need to check IE/EXL before enter exception handling?

        // instruction fetch
        instruction_ = mmu_.read_word(pc_, exception);


        // instruction decode
        instruction_decode(exception);

        // TODO: what if exception is already triggered?

        // timer interrupt
        // interrupt will be handled after the current instruction
        if (cp0_.registers_[cp0_.Count] >= cp0_.registers_[cp0_.Compare]) {
            cp0_.set_interrupt_code(cp0_.IP_7);

            if (cp0_.interrupt_enabled() && /* IE and EXL */
                cp0_.interrupt_enabled(cp0_.IP_7) /* timer interrupt is enabled */) {
                cp0_.set_exception_code(cp0_.Exc_Int);
                exception = true;
                return;
            }
        }
    }

    void instruction_decode(bool& exception);

    void exe_sll(bool& exception); 
    void exe_srl(bool& exception); 
    void exe_sra(bool& exception); 
    void exe_sllv(bool& exception); 
    void exe_srlv(bool& exception);
    void exe_srav(bool& exception); 
    void exe_jr(bool& exception);
    void exe_jalr(bool& exception);
    void exe_syscall(bool& exception);
    void exe_mfhi(bool& exception);
    void exe_mthi(bool& exception);
    void exe_mflo(bool& exception);
    void exe_mtlo(bool& exception);
    void exe_mult(bool& exception);
    void exe_addu(bool& exception);
    void exe_subu(bool& exception);
    void exe_and(bool& exception);
    void exe_or(bool& exception);
    void exe_xor(bool& exception);
    void exe_nor(bool& exception);
    void exe_slt(bool& exception);
    void exe_sltu(bool& exception);
    void exe_bltz(bool& exception);
    void exe_bgez(bool& exception);
    void exe_j(bool& exception);
    void exe_jal(bool& exception);
    void exe_beq(bool& exception);
    void exe_bne(bool& exception);
    void exe_blez(bool& exception);
    void exe_bgtz(bool& exception);
    void exe_addiu(bool& exception);
    void exe_slti(bool& exception);
    void exe_sltiu(bool& exception);
    void exe_andi(bool& exception);
    void exe_ori(bool& exception);
    void exe_xori(bool& exception);
    void exe_lui(bool& exception);
    void exe_mfc0(bool& exception);
    void exe_mtc0(bool& exception);
    void exe_tlbwi(bool& exception);
    void exe_eret(bool& exception);
    void exe_lb(bool& exception);
    void exe_lw(bool& exception);
    void exe_lbu(bool& exception);
    void exe_lhu(bool& exception);
    void exe_sb(bool& exception);
    void exe_sw(bool& exception);
    void exe_cache(bool& exception);


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

#endif /* CPU_ */
