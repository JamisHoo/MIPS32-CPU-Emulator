#include <cstdint>

struct CP0 {
    CP0() {
        // TODO: initialize some reg
    }

    enum { 
        SR = 12, 
        Cause = 13,
        EPC = 14,
        Count = 9,
        Compare = 11,
        BadVAddr = 8,
        Context = 4,
        EntryHi = 10,
        EntryLo0 = 2,
        EntryLo1 = 3,
        Index = 0,
        EBase = 15
    };

    uint32_t registers_[32];
};
