#ifndef PROCESSOR
#define PROCESSOR
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define BROJ_REGISTARA 7
#define MAX_ADDRESS 65536
#define BROJ_ZASTAVA 6

typedef unsigned char BYTE;
typedef unsigned short WORD;
enum zastave{CARRY, AUX_CARRY, SIGN, ZERO, PARITY, INTE};     //redni broj zastava
enum reg{B, C, D, E, H, L, A};  //redni broj registara

void exeInst(BYTE* ram, BYTE* registri, WORD* stekPointer, WORD* programCounter, bool* flags);

bool setParity(BYTE vrednost);
bool setCarry(WORD vrednost);
bool setAuxCarry(BYTE vrednost);
bool setSign(BYTE vrednost);
bool setZero(BYTE vrednost);

void showStats(BYTE* ram, BYTE* registri, WORD* stekPointer, WORD* programCounter, bool* flags, bool mode);

WORD readMachineCode(char* imeFajla, BYTE* code);

void runProgramFromTemp(void);
#endif // PROCESSOR
