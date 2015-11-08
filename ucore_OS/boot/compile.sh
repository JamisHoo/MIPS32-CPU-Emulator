/root/mips-2014.05/bin/mips-linux-gnu-gcc -c -EL bootasm.S
/root/mips-2014.05/bin/mips-linux-gnu-ld -EL -Ttext 0x00 -o bootasm.elf bootasm.o
/root/mips-2014.05/bin/mips-linux-gnu-objcopy -j.text -O binary bootasm.elf bootasm.bin
rm -f bootasm.o
