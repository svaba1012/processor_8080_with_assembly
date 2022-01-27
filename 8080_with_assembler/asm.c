#include "asm.h"
void readCode(char* imeFajla, char* code){
    long long sizeF;
    FILE* fajl;
    fajl = fopen(imeFajla, "r");
    if(fajl == NULL){
        printf("Nepostojeci fajl\n");
        exit(0);
    }
    fseek(fajl, 0, SEEK_END);
    sizeF = ftell(fajl) - 1;
    fseek(fajl, 0, SEEK_SET);
    fread(code, 1, sizeF, fajl);
    code[sizeF - 1] = '\0';
    fclose(fajl);
}

void preprocesor(char* code){
    char* p = code;
    char* q = code;
    while(*p){
        while(*p != ';'){
            *q++ = *p++;
        }
        *q++ = *p;
        while(*p++ != '\n'){
            if(*p == '\0')
                break;
        }
    }
    *q = '\0';
}

void asembler(char* code, BYTE* machineCode, int* numOfInst){
    char* qur = code;
    char *prev = code;
    char line[MAX_LINE_LENGHT];
    int numLine = 1;
    while(*(qur)){
        qur = strchr(qur, ';');
        strncpy(line, prev, qur - prev + 1);
        line[qur - prev + 1] = '\0';
        //printf(line);
        //printf("\n");
        decodeLine(line, numLine, machineCode, numOfInst);
        numLine++;
        prev = ++qur;
        //printf("%s", prev);
    }
}

void decodeLine(char* line, int numLine, BYTE* machineCode, int* numOfInst){
    char* qur = line;
    char opcode[MAX_OPCODE_LENGHT], operators[30], helpOper[30];
    BYTE instrukcija;
    WORD dataOfInst;
    const char noOIndex[NO_O] = {0, 7, 11, 12, 13, 15, 17, 19,
    21, 23, 32, 40, 41, 46, 51, 56, 58, 61, 62, 64, 67,
    69, 72, 73, 75};
    const char regPairOIndex[REGPAIR_O] = {2, 37, 33, 8, 9, 3};
    const char regDestOIndex[REGD_O] = {6, 4, 5};
    const char regSourceOIndex[REGS_O] = {24, 25, 26, 27, 28, 29, 30, 31};
    const char data8OIndex[DATA8_O] = {38, 45, 48, 50, 55, 53, 60, 66, 71, 77};
    const char adrOIndex[ADR_O] = {14, 16, 18, 20, 34, 35, 36, 42, 43, 44, 47,
    49, 52, 54, 57, 59, 63, 65, 68, 70, 74, 76};
    const char allOpcodes[NUM_INST][MAX_OPCODE_LENGHT] = {"NOP", "LXI",
    "STAX", "INX", "INR", "DCR", "MVI", "RLC", "DAD",
    "LDAX", "DCX", "RRC", "RAL", "RAR", "SHLD", "DAA",
    "LHLD", "CMA", "STA", "STC", "LDA", "CMC", "MOV",
    "HLT", "ADD", "ADC", "SUB", "SBB", "ANA", "XRA",
    "ORA", "CMP", "RNZ", "POP", "JNZ", "JMP", "CNZ",
    "PUSH", "ADI", "RST", "RZ", "RET", "JZ", "CZ",
    "CALL", "ACI", "RNC", "JNC", "OUT", "CNC", "SUI",
    "RC", "JC", "IN", "CC", "SBI", "RPO", "JPO", "XTHL",
    "CPO", "ANI", "RPE", "PCHL", "JPE", "XCHG", "CPE",
    "XRI", "RP", "JP", "DI", "CP", "ORI", "RM", "SPHL",
    "JM", "EI", "CM", "CPI"};
    const BYTE machineInst[NUM_INST] = {0x00, 0x01, 0x02,
    0x03, 0x04, 0x05, 0x06, 0x07, 0x09, 0x0a, 0x0b, 0x0f,
    0x17, 0x1f, 0x22, 0x27, 0x2a, 0x2f, 0x32, 0x37, 0x3a,
    0x3f, 0x40, 0x76, 0x80, 0x88, 0x90, 0x98, 0xa0, 0xa8,
    0xb0, 0xb8, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6,
    0xc7, 0xc8, 0xc9, 0xca, 0xcc, 0xcd, 0xce, 0xd0, 0xd2,
    0xd3, 0xd4, 0xd6, 0xd8, 0xda, 0xdb, 0xdc, 0xde, 0xe0,
    0xe2, 0xe3, 0xe4, 0xe6, 0xe8, 0xe9, 0xea, 0xeb, 0xec,
    0xee, 0xf0, 0xf2, 0xf3, 0xf4, 0xf6, 0xf8, 0xf9, 0xfa,
    0xfb, 0xfc, 0xfe};
    qur = strchr(line, ' ');
    if(qur == NULL){
        strcpy(opcode, line);
        opcode[strlen(opcode) - 1] = '\0';
        operators[0] = '\0';
    }else{
        strncpy(opcode, line, qur + 1 - line);
        opcode[qur++ - line] = '\0';
        strcpy(operators, qur);
        operators[strlen(qur) - 1] = '\0';
    }
    int i;
    for(i = 0; i < NUM_INST; i++){
        if(strcmp(opcode, allOpcodes[i]) == 0){
            instrukcija = machineInst[i];
            if(i == 22){//MOV instrukcija
                regOperSandD(operators, &instrukcija, numLine);
                machineCode[(*numOfInst)++] = instrukcija;
            }else if(i == 1){ //LXI instrukcija
                char* spec;
                spec = strchr(operators, ',');
                if(spec == NULL){
                    printf("Pogresni operator, linija: %d\n", numLine);
                    exit(0);
                }
                strncpy(helpOper, operators, spec + 1 - operators);
                helpOper[spec++ - operators] = '\0';
                regPairOper(helpOper, &instrukcija, numLine);
                machineCode[(*numOfInst)++] = instrukcija;
                if(*spec == ' '){
                    dataOfInst = dataDecode(spec + 1, numLine, 1);
                    machineCode[(*numOfInst)++] = dataOfInst % 256;
                    machineCode[(*numOfInst)++] = dataOfInst / 256;
                }else{
                    printf("Pogresni operator, linija: %d\n", numLine);
                    exit(0);
                }
            }else if(i == 6){ //MVI instrukcija
                char* spec;
                spec = strchr(operators, ',');
                if(spec == NULL){
                    printf("Pogresni operator, linija: %d\n", numLine);
                    exit(0);
                }
                strncpy(helpOper, operators, spec + 1 - operators);
                helpOper[spec++ - operators] = '\0';
                regOperDest(helpOper, &instrukcija, numLine);
                machineCode[(*numOfInst)++] = instrukcija;
                if(*spec == ' '){
                    dataOfInst = dataDecode(spec + 1, numLine, 0);
                    machineCode[(*numOfInst)++] = dataOfInst;
                }else{
                    printf("Pogresni operator, linija: %d\n", numLine);
                    exit(0);
                }
            }else if(i == 39){//RST instrukcija
                if((strlen(operators) == 1) && (operators[0] >= '0') && (operators[0] <= '7')){
                    instrukcija |= (operators[0] - '0') << 3;
                    machineCode[(*numOfInst)++] = instrukcija;
                }else{
                    printf("Pogresni operator, linija: %d\n", numLine);
                    exit(0);
                }
            }else{
                for(int j = 0; j < NO_O; j++){
                    if(i == noOIndex[j]){
                        if(strlen(operators) > 0){
                            printf("Pogresni operator, linija: %d\n", numLine);
                            exit(0);
                        }
                        machineCode[(*numOfInst)++] = instrukcija;
                    }else if((j < REGPAIR_O) && (i == regPairOIndex[j])){
                        regPairOper(operators, &instrukcija, numLine);
                        machineCode[(*numOfInst)++] = instrukcija;
                    }else if((j < REGD_O) && (i == regDestOIndex[j])){
                        regOperDest(operators, &instrukcija, numLine);
                        machineCode[(*numOfInst)++] = instrukcija;
                    }else if((j < REGS_O) && (i == regSourceOIndex[j])){
                        regOperSorce(operators, &instrukcija, numLine);
                        machineCode[(*numOfInst)++] = instrukcija;
                    }else if((j < DATA8_O) && (i == data8OIndex[j])){
                        machineCode[(*numOfInst)++] = instrukcija;
                        dataOfInst = dataDecode(operators, numLine, 0);
                        machineCode[(*numOfInst)++] = dataOfInst;
                    }else if((j < ADR_O) && (i == adrOIndex[j])){
                        machineCode[(*numOfInst)++] = instrukcija;
                        dataOfInst = dataDecode(operators, numLine, 1);
                        machineCode[(*numOfInst)++] = dataOfInst % 256;
                        machineCode[(*numOfInst)++] = dataOfInst / 256;
                    }
                }
            }
            break;
        }
    }
    if(i == NUM_INST){
        printf("Nepostojeca instrukcija, linija: %d\n", numLine);
        exit(0);
    }
}


void regPairOper(char* operators, BYTE* instrukcija, int numLine){
    if(strcmp(operators, "D") == 0){
        *instrukcija |= 0x10;
    }else if((*instrukcija != 0x02) && (*instrukcija != 0x0a) && (strcmp(operators, "H") == 0)){// instrukcije STAX i LDAX nemaju H operator
        *instrukcija |= 0x20;
    }else if(((*instrukcija == 0xc1) || (*instrukcija == 0xc5)) && (strcmp(operators, "PSW") == 0)){//instrukcije POP i PUSH imaju operator PSW
        *instrukcija |= 0x30;
    }else if(((*instrukcija == 0x1) || (*instrukcija == 0x3) || (*instrukcija == 0xb) || (*instrukcija == 0x9)) && (strcmp(operators, "SP") == 0)){ //instrukcije DAD, LXI, INX i DCX imaju SP operator
        *instrukcija |= 0x30;
    }else if(strcmp(operators, "B") != 0){
        printf("Pogresan operator, linija: %d\n", numLine);
        exit(0);
    }
}

void regOperSorce(char* operators, BYTE* instrukcija, int numLine){
    if(strcmp(operators, "C") == 0){
        *instrukcija |= 0x1;
    }else if((strcmp(operators, "D") == 0)){
        *instrukcija |= 0x2;
    }else if((strcmp(operators, "E") == 0)){
        *instrukcija |= 0x3;
    }else if((strcmp(operators, "H") == 0)){
        *instrukcija |= 0x4;
    }else if((strcmp(operators, "L") == 0)){
        *instrukcija |= 0x5;
    }else if((strcmp(operators, "M") == 0)){
        *instrukcija |= 0x6;
    }else if((strcmp(operators, "A") == 0)){
        *instrukcija |= 0x7;
    }else if(strcmp(operators, "B") != 0){
        printf("Pogresan operator, linija: %d\n", numLine);
        exit(0);
    }
}

void regOperDest(char* operators, BYTE* instrukcija, int numLine){
    if(strcmp(operators, "C") == 0){
        *instrukcija |= 0x8;
    }else if((strcmp(operators, "D") == 0)){
        *instrukcija |= 0x10;
    }else if((strcmp(operators, "E") == 0)){
        *instrukcija |= 0x18;
    }else if((strcmp(operators, "H") == 0)){
        *instrukcija |= 0x20;
    }else if((strcmp(operators, "L") == 0)){
        *instrukcija |= 0x28;
    }else if((strcmp(operators, "M") == 0)){
        *instrukcija |= 0x30;
    }else if((strcmp(operators, "A") == 0)){
        *instrukcija |= 0x38;
    }else if(strcmp(operators, "B") != 0){
        printf("Pogresan operator, linija: %d\n", numLine);
        exit(0);
    }
}

void regOperSandD(char* operators, BYTE* instrukcija, int numLine){
    char operDest[2];
    char operSorce[2];
    operDest[0] = operators[0];
    operDest[1] = '\0';
    operSorce[0] = operators[strlen(operators) - 1];
    operSorce[1] = '\0';
    if((strlen(operators) == 4) && (operators[1] == ',') && (operators[2] == ' ')){
        regOperSorce(operSorce, instrukcija, numLine);
        regOperDest(operDest, instrukcija, numLine);
    }else{
        printf("Pogresan operator, linija: %d\n", numLine);
        exit(0);
    }

}

WORD dataDecode(char* operators, int numLine, int mode){
    int data = 0;
    char zadnji;
    int granica = 256;
    zadnji = operators[strlen(operators) - 1];
    switch(zadnji){
        case 'B':   operators[strlen(operators) - 1] = '\0';
                    if(strlen(operators) == 0){
                        printf("Pogresni operator, linija: %d\n", numLine);
                        exit(0);
                    }
                    for(char* ptr = operators; *ptr; ptr++){
                        if((*ptr >= '0') && (*ptr <= '1')){
                            data *= 2;
                            data += *ptr - '0';
                        }else{
                            printf("Pogresni operator, linija: %d\n", numLine);
                            exit(0);
                        }
                    }
                    break;
        case 'H':   operators[strlen(operators) - 1] = '\0';
                    if(strlen(operators) == 0){
                        printf("Pogresni operator, linija: %d\n", numLine);
                        exit(0);
                    }
                    for(char* ptr = operators; *ptr; ptr++){
                        if((*ptr >= '0') && (*ptr <= '9')){
                            data *= 16;
                            data += *ptr - '0';
                        }else if((*ptr >= 'A') && (*ptr <= 'F')){
                            data *= 16;
                            data += *ptr - 'A' + 10;
                        }else{
                            printf("Pogresni operator, linija: %d\n", numLine);
                            exit(0);
                        }
                    }
                    break;
        case 'O':   operators[strlen(operators) - 1] = '\0';
                    if(strlen(operators) == 0){
                        printf("Pogresni operator, linija: %d\n", numLine);
                        exit(0);
                    }
                    for(char* ptr = operators; *ptr; ptr++){
                        if((*ptr >= '0') && (*ptr <= '7')){
                            data *= 8;
                            data += *ptr - '0';
                        }else{
                            printf("Pogresni operator, linija: %d", numLine);
                            exit(0);
                        }
                    }
                    break;
        default:    if(((zadnji >= '0') && (zadnji <= '9')) || (zadnji == 'D')){
                        if(zadnji == 'D')
                            operators[strlen(operators) - 1] = '\0';
                        if(strlen(operators) == 0){
                            printf("Pogresni operator, linija: %d\n", numLine);
                            exit(0);
                        }
                        for(char* ptr = operators; *ptr; ptr++){
                            if((*ptr >= '0') && (*ptr <= '9')){
                                data *= 10;
                                data += *ptr - '0';
                            }else{
                                printf("Pogresni operator, linija: %d\n", numLine);
                                exit(0);
                            }
                        }
                    }else{
                        printf("Pogresni operator, linija: %d\n", numLine);
                        exit(0);
                    }
    }
    if(mode)
        granica = 65536;
    if(data >= granica){
        printf("Pogresni operator, linija: %d\n", numLine);
        exit(0);
    }
    return data;
}

void saveMC(char* nameOfFile, char* code, int n){
    FILE* fajl;
    fajl = fopen(nameOfFile, "w");
    fwrite(code, 1, n, fajl);
    fclose(fajl);
}

void fullAsm(void){
    char imeFajla[MAX_LINE_LENGHT];
    char fullIme[MAX_LINE_LENGHT];
    BYTE machineCode[MAX_INST_NUM];
    char code[MAX_AS_CODE_LENGHT];
    int numOfInst = 0;
    char c;
    FILE* fajlDest;
    printf("-----------------------Svaba8080_asm---------------------------\n");
    printf("------------------------Verzija 1.1----------------------------\n");
    printf("Ako zelite da asemblujete fajl temp.exe unesite nulu(0),\nako zelite neki drugi unesite bilo sta\n");
    scanf("%c", &c);
    if(c == '0'){
        strcpy(fullIme, ".\\tempAsm.txt");
    }else{
        printf("Unesite ime fajla za asemblovanje\n");
        scanf("%s", imeFajla);
        sprintf(fullIme, ".\\%s", imeFajla);
    }
    //printf(fullIme);
    readCode(fullIme, code);
    preprocesor(code);
    //printf(code);
    asembler(code, machineCode, &numOfInst);
    for(int i = 0; i < numOfInst; i++)
        printf("0x%x\n", machineCode[i]);
    fajlDest = fopen(".\\tempMachine.b", "wb");
    fwrite(machineCode, 1, numOfInst, fajlDest);
    fclose(fajlDest);
    printf("Uspesno asemblovano!\n");
    printf("--------------------------------------------------------------\n\n\n");
}
