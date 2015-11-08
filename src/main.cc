#include <fstream>
#include <sstream>

#include "cpu.h"

int main() {
    using namespace std;
    
    ifstream rom("OS_image/rom");
    ifstream flash("OS_image/disk0");

    // copy files to memory to speed up access
    stringstream rom_buffer;
    stringstream flash_buffer;
    copy(istreambuf_iterator<char>(rom), istreambuf_iterator<char>(), 
         ostreambuf_iterator<char>(rom_buffer));
    copy(istreambuf_iterator<char>(flash), istreambuf_iterator<char>(), 
         ostreambuf_iterator<char>(flash_buffer));

    CPU cpu(rom_buffer, flash_buffer);

    cpu.run();

}

