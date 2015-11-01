#ifndef MMU_
#define MMU_

#include <iostream>
#include <fstream>

#include "cp0.h"


class MMU {
public:
    MMU(CP0& cp0, std::istream& rom, std::istream& flash):
        cp0_(cp0), rom_(rom), flash_(flash) {
        ram_.resize(RAM_SIZE);    
        // need not initialize TLB
    }

    uint8_t read_byte(uint32_t virtual_addr, bool& exception) {
        // TODO: Exception: access kernel space from user mode?
        uint32_t physical_addr = addr_translate(virtual_addr, 0, exception);
        if (exception) return 0x00;
        return read_physical(physical_addr); 
    }

    uint16_t read_half_word(uint32_t virtual_addr, bool& exception) {
        if (virtual_addr & 0x01) {
            // load from an address that is not aligned on a halfword boundary
            cp0_.set_exception_code(cp0_.Exc_AdEL);
            cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
            exception = true;
            return 0x00;
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 0, exception);
        if (exception) return 0x00;
        return uint16_t(read_physical(physical_addr + 0)) | 
               uint16_t(read_physical(physical_addr + 1)) << 8;

    }

    uint32_t read_word(uint32_t virtual_addr, bool& exception) {
        if (virtual_addr & 0x03) {
            // load or instruction fetch from an address that is not aligned on a word boundary
            cp0_.set_exception_code(cp0_.Exc_AdEL);
            cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
            exception = true;
            return 0x00;
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 0, exception);
        if (exception) return 0x00;
        return uint32_t(read_physical(physical_addr + 0))       |
               uint32_t(read_physical(physical_addr + 1)) <<  8 |
               uint32_t(read_physical(physical_addr + 2)) << 16 |
               uint32_t(read_physical(physical_addr + 3)) << 24;

    }

    void write_byte(uint32_t virtual_addr, uint8_t data, bool& exception) {
        uint32_t physical_addr = addr_translate(virtual_addr, 1, exception);
        if (exception) return;
        write_physical(physical_addr, data);
    }

    void write_half_word(uint32_t virtual_addr, uint16_t data, bool& exception) { 
        if (virtual_addr & 0x01) {
            // save to an address that is not aligned on a halfword boundary
            cp0_.set_exception_code(cp0_.Exc_AdES);
            cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
            exception = true;
            return;
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 1, exception);
        if (exception) return;
        write_physical(physical_addr + 0, uint8_t(data));
        write_physical(physical_addr + 1, uint8_t(data >> 8));
    }

    void write_word(uint32_t virtual_addr, uint32_t data, bool& exception) {
        if (virtual_addr & 0x03) {
            // save to an address that is not aligned on a word boundary
            cp0_.set_exception_code(cp0_.Exc_AdES);
            cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
            exception = true;
            return;
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 1, exception);
        if (exception) return;
        write_physical(physical_addr + 0, uint8_t(data));
        write_physical(physical_addr + 1, uint8_t(data >>  8));
        write_physical(physical_addr + 2, uint8_t(data >> 16));
        write_physical(physical_addr + 3, uint8_t(data >> 24));
    }
    
private:
    uint8_t read_physical(uint32_t addr) {
        // access ram
        if (addr >= RAM_BASE && addr < RAM_BASE + RAM_SIZE)
            return ram_[addr - RAM_BASE];

        // access rom
        if (addr >= ROM_BASE && addr < ROM_BASE + ROM_SIZE) {
            rom_.seekg(addr - ROM_BASE);
            uint8_t val;
            rom_.read((char*)(&val), 1);
            return val;
        }

        // access flash
        if (addr >= FLASH_BASE && addr < FLASH_BASE + FLASH_SIZE) {
            flash_.seekg(addr - FLASH_BASE);
            uint8_t val;
            flash_.read((char*)(&val), 1);
            return val;
        }

        // access serial port 
        if (addr == SERIAL_PORT) {
            // TODO: read from stdin
        }
            
        if (addr == SERIAL_STATUS) {
            return 0x01;
        }
        

        // other space
        return 0x00;
    }

    void write_physical(uint32_t addr, uint8_t data) {
        // access ram
        if (addr >= RAM_BASE && addr < RAM_BASE + RAM_SIZE)
            ram_[addr - RAM_BASE] = data;

        if (addr == SERIAL_PORT) {
            // to stdout
            std::cout.put(data);
            std::cout.flush();
        }

        // other space
    }

    uint32_t addr_translate(uint32_t virtual_addr, bool write, bool& exception) {
        // kseg0, unmapped
        if (virtual_addr >= KSEG0_BASE && virtual_addr < KSEG0_BASE + KSEG0_SIZE) 
            return virtual_addr & 0x1fffffff;
        
        // kseg1, unmapped
        if (virtual_addr >= KSEG1_BASE && virtual_addr < KSEG1_BASE + KSEG1_SIZE) 
            return virtual_addr & 0x1fffffff;

        // kuseg or kseg2, mapped
        uint32_t vpn2 = virtual_addr >> 13;
        for (uint32_t i = 0; i < k_TLB_ENTRIES; ++i) 
            if (tlb_key_[i] >> 13 == vpn2) {
                uint32_t entry_lo = tlb_data_[virtual_addr >> 12 & 0x01][i];
                // V bit
                if (!(entry_lo >> 1 & 0x01)) {
                    // TLB Invalid exception
                    cp0_.set_exception_code(write? cp0_.Exc_TLBS: cp0_.Exc_TLBL);
                    cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
                    cp0_.registers_[cp0_.Context] &= 0xff80000f;
                    cp0_.registers_[cp0_.Context] |= virtual_addr >> 13 << 4;
                    cp0_.registers_[cp0_.EntryHi] &= 0x00001fff; 
                    cp0_.registers_[cp0_.EntryHi] |= virtual_addr & 0xffffe000;
                    exception = true;
                    return 0x00;
                }
                // D bit
                if (!(entry_lo >> 2 & 0x01) && write) {
                    // TLB  Modified exception
                    cp0_.set_exception_code(cp0_.Exc_Mod);
                    cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
                    cp0_.registers_[cp0_.Context] &= 0xff80000f;
                    cp0_.registers_[cp0_.Context] |= virtual_addr >> 13 << 4;
                    cp0_.registers_[cp0_.EntryHi] &= 0x00001fff; 
                    cp0_.registers_[cp0_.EntryHi] |= virtual_addr & 0xffffe000;
                    exception = true;
                    return 0x00;
                }
                    
                return (entry_lo << 6 & 0xfffff000) | (virtual_addr & 0x00000fff);
            }
        
        // TLB refill exception
        cp0_.set_exception_code(write? cp0_.Exc_TLBS: cp0_.Exc_TLBL);
        cp0_.registers_[cp0_.BadVAddr] = virtual_addr;
        cp0_.registers_[cp0_.Context] &= 0xff80000f;
        cp0_.registers_[cp0_.Context] |= virtual_addr >> 13 << 4;
        cp0_.registers_[cp0_.EntryHi] &= 0x00001fff; 
        cp0_.registers_[cp0_.EntryHi] |= virtual_addr & 0xffffe000;
        exception = true;
        return 0x00;
    }

    // virtual memory layout
    constexpr static uint32_t KUSEG_BASE = 0;
    constexpr static uint32_t KUSEG_SIZE = 0x80000000;

    constexpr static uint32_t KSEG0_BASE = 0x80000000;
    constexpr static uint32_t KSEG0_SIZE = 0x20000000;

    constexpr static uint32_t KSEG1_BASE = 0xa0000000;
    constexpr static uint32_t KSEG1_SIZE = 0x20000000;

    constexpr static uint32_t KSEG2_BASE = 0xc0000000;
    constexpr static uint32_t KSEG2_SIZE = 0x40000000;

    // physical memory layout
    constexpr static uint32_t RAM_BASE = 0;
    constexpr static uint32_t RAM_SIZE = 1024 * 1024 * 8;  // 8 MB

    constexpr static uint32_t ROM_BASE = 0x1fc00000;
    constexpr static uint32_t ROM_SIZE = 1024 * 4;  // 4 KB

    constexpr static uint32_t FLASH_BASE = 0x1e000000;
    constexpr static uint32_t FLASH_SIZE = 1024 * 1024 * 8;  // 8 MB

    constexpr static uint32_t SERIAL_PORT = 0x1fd003f8;
    constexpr static uint32_t SERIAL_STATUS = 0x1fd003fc;

    std::string ram_;
    std::istream& rom_;
    std::istream& flash_;

    CP0& cp0_;

    constexpr static uint32_t k_TLB_ENTRIES = 16;
public:
    // TLB key: VPN2(19 bits) | 0(5 bits) | ASID(8 bits)
    uint32_t tlb_key_[k_TLB_ENTRIES];
    // TLB data: PFN(26 bits) | C(3 bits) | D(1bit) | V(1bit) | G(1bit)
    uint32_t tlb_data_[2][k_TLB_ENTRIES];
};

#endif /* MMU_ */
