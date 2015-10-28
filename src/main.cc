#include <fstream>

#include "cpu.h"

int main() {
    using namespace std;
    
    ifstream rom("../image/rom");
    ifstream flash("../image/kernel");

    CPU cpu(rom, flash);

    cpu.run();

}

