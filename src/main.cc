#include <fstream>

#include "cpu.h"

int main() {
    using namespace std;
    
    ifstream rom("../image/rom");
    ifstream flash("../image/disk0");

    CPU cpu(rom, flash);

    cpu.run();

}

