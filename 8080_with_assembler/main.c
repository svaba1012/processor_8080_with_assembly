#include <stdio.h>
#include <stdlib.h>
#include "8080.h"
#include "asm.h"

int main(){
    fullAsm(); //assemble the program in ./tempAsm.txt
    runProgramFromTemp();
    return 0;
}
