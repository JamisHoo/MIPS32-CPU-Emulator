#include "cp0.h"
#include "mem.h"

class CPU {
    
    uint32_t IF() {


    }

    void ID() {


    }

    void EXE() {

    }

    void MEM() {

    }

    void WB() {

    }


private:
    CP0 cp0_;
    Memory mem_;

    uint32_t registers[32];
    uint32_t PC;
};
