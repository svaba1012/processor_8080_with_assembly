#ifndef ASM
#define ASM
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_INST 78
#define NO_O 25
#define REGPAIR_O 6
#define REGD_O 3
#define REGS_O 8
#define DATA8_O 10
#define ADR_O 22
#define MAX_LINE_LENGHT 30
#define MAX_AS_CODE_LENGHT 10000
#define MAX_OPCODE_LENGHT 5
#define MAX_INST_NUM 10000

typedef unsigned char BYTE;
typedef unsigned short WORD;

void readCode(char* imeFajla, char* code);

void preprocesor(char* code);

void asembler(char* code, BYTE* machineCode, int* numOfInst);
void decodeLine(char* line, int numLine, BYTE* machineCode, int* numOfInst);
void regPairOper(char* operators, BYTE* instrukcija, int numLine);
void regOperSorce(char* operators, BYTE* instrukcija, int numLine);
void regOperDest(char* operators, BYTE* instrukcija, int numLine);
void regOperSandD(char* operators, BYTE* instrukcija, int numLine);
WORD dataDecode(char* operators, int numLine, int mode);

void saveMC(char* imeFajla, char* code, int n);

void fullAsm(void);

#endif // ASM
