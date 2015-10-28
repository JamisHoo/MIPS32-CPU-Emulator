#include <iostream>
#include <fstream>

class MMU {
public:
    MMU(std::ifstream& rom, std::ifstream& flash):
        rom_(rom), flash_(flash) {
        ram_.resize(RAM_SIZE);    
        // TODO: need initialize TLB?
    }

    uint8_t read_byte(uint32_t virtual_addr) {
        uint32_t physical_addr = addr_translate(virtual_addr, 0);
        return read_physical(physical_addr); 
    }

    uint16_t read_half_word(uint32_t virtual_addr) {
        if (virtual_addr & 0x01) {
            // TODO: alignment exception
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 0);
        return uint16_t(read_physical(physical_addr + 0)) | 
               uint16_t(read_physical(physical_addr + 1)) << 8;

    }

    uint32_t read_word(uint32_t virtual_addr) {
        if (virtual_addr & 0x03) {
            // TODO: alignment exception
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 0);
        return uint32_t(read_physical(physical_addr + 0))       |
               uint32_t(read_physical(physical_addr + 1)) <<  8 |
               uint32_t(read_physical(physical_addr + 2)) << 16 |
               uint32_t(read_physical(physical_addr + 3)) << 24;

    }

    void write_byte(uint32_t virtual_addr, uint8_t data) {
        uint32_t physical_addr = addr_translate(virtual_addr, 1);
        write_physical(physical_addr, data);
    }

    void write_half_word(uint32_t virtual_addr, uint16_t data) { 
        if (virtual_addr & 0x01) {
            // TODO: alignment exception
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 1);
        write_physical(physical_addr + 0, uint8_t(data));
        write_physical(physical_addr + 1, uint8_t(data >> 8));
    }

    void write_word(uint32_t virtual_addr, uint32_t data) {
        if (virtual_addr & 0x11) {
            // TODO: alignment exception
        }
        uint32_t physical_addr = addr_translate(virtual_addr, 1);
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
        if (addr >= FLASH_BASE && addr < FLASH_SIZE) {
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
            // TODO: handle serial port 
            return 0x01;
        }
        

        // other space
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

    uint32_t addr_translate(uint32_t virtual_addr, bool write) {
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
                if (entry_lo >> 1 & 0x01) {
                    // TODO: exception
                }
                // D bit
                if (!(entry_lo >> 2 & 0x01) && write) {
                    // TODO: exception
                }
                    
                return (entry_lo << 6 & 0xfffff000) | (virtual_addr & 0x00000fff);
            }
        
        // TODO: TLB miss exception
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
    std::ifstream& rom_;
    std::ifstream& flash_;

    constexpr static uint32_t k_TLB_ENTRIES = 16;
public:
    // TLB key: VPN2(19 bits) | 0(5 bits) | ASID(8 bits)
    uint32_t tlb_key_[k_TLB_ENTRIES];
    // TLB data: PFN(26 bits) | C(3 bits) | D(1bit) | V(1bit) | G(1bit)
    uint32_t tlb_data_[2][k_TLB_ENTRIES];
};
