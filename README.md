# MIPS32 CPU Emulator

This is a MIPS32 instruction subset based CPU emulator. It supports over 40 instructions and is capable of running a Unix-like OS [ucore](https://github.com/chyyuu/ucore_os_lab). This emulator is implemented in C++ and completely platform independent. You can compile it to JavaScript with [emscripten](http://emscripten.org) so as to run this emulator in web browsers.


## Setup

You'll need a compiler supporting C++11 to run the emulator on your local 
machine. 
You'll need to install [emscripten](http://emscripten.org) in addition if you want to run it in web browsers.

First, compile the emulator.

```
$ make
```

Then, run the emulator on local machine.

```
$ ./cpu
```

Run the emulator with Node.js (supposing you've already installed Node.js).

```
$ node cpu.js
```

Setup a web server and open cpu.html if you want to run it in browsers. The OS image file is 
kind of big, so you may use some compression techniques.

## About Implementation

### Instructions
Currently support 48 instructions. It's extremely easy to extend to support other instructions.

```
sll, srl, sra, sllv, srlv, srav, 
jr, jalr, 
syscall, 
mfhi, mthi, mflo, mtlo, mult, 
addu, subu, and, or, xor, nor, slt, sltu, 
bltz, bgez, j, jal, beq, bne, blez, bgtz, 
addiu, slti, sltiu, andi, ori, xori, lui, 
mfc0, mtc0, 
tlbwi, 
eret, 
lb, lw, lbu, lhu, sb, sw, 
cache
```

### Address Space

### Program Counter (PC)
PC is initialized to 0xbfc00000 (virtual address). So you should always put ROM at 0x1fc0000 (physical address). This emulator imitates a single-cycle MIPS processor so delay slot is not supported. 

### CP0 Registers

* Index

    There are a total of 16 TLB entries in MMU. So only 4 least significant bits are used.
    
* EntryLo0

    `PFN`, `D`, `V` fields are implemented. The LSBs are used if width of pfn is less than 26 bits.
    
* EntryLo1

    As above.
    
* Context

    `BadVPN2` field is implemented by hardware.
    
* BadVAddr

    Implemented.

* Count

    Count register will increment by one on executing every instruction. 
When this value is equal to Compare register, 
a clock interrupt is triggered, if enabled. 
Hardware will clear Count register after triggering a clock intrrupt,
no matter this interrupt is enabled or not.

* EntryHi
    
    `VPN2` field is implemented.

* Compare
    
    Compare register is initialized by software.

* Status

    `IE`, `EXL`, `IM7-0` fields are implemented in hardware.
`EXL` is set when exceptions occur and is unset when excetuing `eret` instructions. 

* Cause

    `ExcCode`, `IP7-0` fields are implemented in hardware.

* EPC

    Implemented.

* EBase

    Its implementation is kind of different from MIPS32 standard.
EBase will be initialized to 0x80000180 by hardware.
PC will jump to this address when an exception occurs.

Other CP0 registers are not implemented. 

### Exceptions and Interrupts

#### Exceptions
Currently supported exceptions.

* Int (0)
* Mod (1)
* TLBL (2)
* TLBS (3)
* AdEL (4)
* AdES (5)
* Syscall (8)
* RI (10)

#### Interrupts
* clock interrupt (IP_7)

    Implemented. 
    
* serial port interrupt (IP_2)
    
    Not implemented yet.

## References

## License