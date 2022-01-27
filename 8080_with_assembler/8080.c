#include "8080.h"
void exeInst(BYTE* ram, BYTE* registri, WORD* stekPointer, WORD* programCounter, bool* flags){
    WORD trenutnaAdresa = *programCounter;
    bool nadjena = true;
    BYTE instrukcija = ram[trenutnaAdresa], temp;
    WORD addr = ram[*programCounter + 1] | (ram[*programCounter + 2] << 8);//adresa kod 3-bajtovskih instrukcija
    int doubleRez;
    WORD hl, bc, specWord;
    BYTE psw, specByte;
    printf("Instrukcija: 0x%x\n", instrukcija);
    switch(instrukcija){
        case 0x0:   break;  //NOP - nista
        case 0x1:   //LXI B, data - 3 bajta, druga dva podaci koji se ucitavaju u C pa u B
                    registri[C] = ram[++(*programCounter)];
                    registri[B] = ram[++(*programCounter)];
                    break;
        case 0x2:   //STAX B - cuva reg A na adresi iz reg B i C
                    ram[(registri[B] << 8) | registri[C]] = registri[A];
                    break;
        case 0x3:   //INX B - dodaje jedan reg paru BC, ne menja flags
                    registri[C]++;
                    if(registri[C] == 0)
                        registri[B]++;
                    break;

        case 0x7:   //RLC - rotira reg A u levo najveci bit se izbija u ubacuje na najmanji
                    flags[CARRY] = (registri[A] & 0x80) >> 7;
                    registri[A] <<= 1;
                    registri[A] |= flags[CARRY];
                    break;
        case 0x8:   //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x9:   //DAD B - dodaje reg par BC na reg par HL
                    hl = registri[L] | (registri[H] << 8);
                    bc = registri[C] | (registri[B] << 8);
                    doubleRez = hl + bc;
                    flags[CARRY] = (doubleRez > 0xffff);
                    registri[L] = doubleRez & 0xff;
                    registri[H] = (doubleRez & 0xff00) >> 8;
                    break;
        case 0xa:   //LDAX B - ucitava u reg A sa adrese iz reg B i C
                    registri[A] = ram[(registri[B] << 8) | registri[C]];
                    break;
        case 0xb:   //DCX B - umanjuje za 1 reg par BC
                    registri[C]--;
                    if(registri[C] == 0xFF)
                        registri[B]--;
                    break;
        case 0xf:   //RRC - rotira A u desno, tj najmanji bit izbija i ubacuje ga kao najveci
                    flags[CARRY] = registri[A] & 0x1;
                    registri[A] >>= 1;
                    registri[A] |= flags[CARRY] << 7;
                    break;
        case 0x10:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x11:  //LXI D, data - 3 bajta, druga dva podaci koji se ucitavaju u E pa u D
                    registri[E] = ram[++(*programCounter)];
                    registri[D] = ram[++(*programCounter)];
                    break;
        case 0x12:  //STAX D - cuva reg A na adresi iz reg D i E
                    ram[(registri[D] << 8) | registri[E]] = registri[A];
                    break;
        case 0x13:  //INX D - uvecava reg par DE za 1
                    registri[E]++;
                    if(registri[E] == 0)
                        registri[D]++;
                    break;
        case 0x17:  //RAL - rotira A u levo kroz carry
                    temp = (registri[A] & 0x80) >> 7;
                    registri[A] <<= 1;
                    registri[A] |= flags[CARRY];
                    flags[CARRY] = temp;
                    break;
        case 0x18:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x19:  //DAD D - dodaje DE na HL
                    hl = registri[L] | (registri[H] << 8);
                    bc = registri[E] | (registri[D] << 8);
                    doubleRez = hl + bc;
                    flags[CARRY] = (doubleRez > 0xffff);
                    registri[L] = doubleRez & 0xff;
                    registri[H] = (doubleRez & 0xff00) >> 8;
                    break;
        case 0x1a:  //LDAX D - ucitava u reg A sa adrese iz reg D i E
                    registri[A] = ram[(registri[D] << 8) | registri[E]];
                    break;
        case 0x1b:  //DCX D - umanjeje reg par DE za 1
                    registri[E]--;
                    if(registri[E] == 0xFF)
                        registri[D]--;
                    break;
        case 0x1f:  //RAR - rotira A u desno kroz carry
                    temp = registri[A] & 0x1;
                    registri[A] >>= 1;
                    registri[A] |= flags[CARRY] << 7;
                    flags[CARRY] = temp;
                    break;
        case 0x20:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x21:  //LXI H, data - 3 bajta, druga dva  podaci koji se ucitavaju u HL
                    registri[L] = ram[++(*programCounter)];
                    registri[H] = ram[++(*programCounter)];
                    break;
        case 0x22:  //SHLD addr - 3 bajta, cuva L na addr i H na addr + 1
                    ram[addr++] = registri[L];
                    ram[addr++] = registri[H];
                    break;
        case 0x23:  //INX H - par HL++
                    registri[L]++;
                    if(registri[L] == 0)
                        registri[H]++;
                    break;
        case 0x27:  //DAA - reg A pretvara u dvocifreni dekadni broj
                    if(flags[AUX_CARRY] || ((registri[A] & 0xf) > 9)){
                        specByte = (registri[A] & 0xf) + 6;
                        specWord = registri[A] + 6;
                        registri[A] = specWord & 0xff;
                        flags[CARRY] = setCarry(specWord);
                        flags[AUX_CARRY] = setAuxCarry(specByte);
                    }
                    if(flags[CARRY] || (((registri[A] & 0xf0) >> 4) > 9)){
                        specWord = registri[A] + 0x60;
                        registri[A] = specWord & 0xff;
                        flags[CARRY] = setCarry(specWord);
                    }
                    flags[PARITY] = setParity(registri[A]);
                    flags[ZERO] = setZero(registri[A]);
                    flags[SIGN] = setSign(registri[A]);
                    break;
        case 0x28:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x29:  //DAD H - HL += HL
                    hl = registri[L] | (registri[H] << 8);
                    doubleRez = hl << 1;
                    flags[CARRY] = (doubleRez > 0xffff);
                    registri[L] = doubleRez & 0xff;
                    registri[H] = (doubleRez & 0xff00) >> 8;
                    break;
        case 0x2a:  //LHLD addr - 3 bajta, ucitava u L i H
                    registri[L] = ram[addr++];
                    registri[H] = ram[addr++];
                    break;
        case 0x2b:  //DCX H - HL--
                    registri[L]--;
                    if(registri[L] == 0xFF)
                        registri[H]--;
                    break;
        case 0x2f:  //CMA - negira reg A
                    registri[A] = ~registri[A];
                    break;
        case 0x30:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x31:  //LXI SP, data - 3 bajta, SP.h postaje data.h, SP.l postaje sa data.l
                    *stekPointer = ram[*programCounter + 1] | (ram[*programCounter + 2] << 8);
                    (*programCounter) += 2;
                    break;
        case 0x32:  //STA addr - 3 bajta, cuva A na addr
                    ram[addr] = registri[A];
                    break;
        case 0x33:  //INX SP - SP++
                    (*stekPointer)++;
                    break;
        case 0x37:  //STC - postavlja carry bit
                    flags[CARRY] = 1;
                    break;
        case 0x38:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0x39:  //DAD SP - HL += SP
                    hl = registri[L] | (registri[H] << 8);
                    doubleRez = hl + *stekPointer;
                    flags[CARRY] = (doubleRez > 0xffff);
                    registri[L] = doubleRez & 0xff;
                    registri[H] = (doubleRez & 0xff00) >> 8;
                    break;
        case 0x3a:  //LDA addr - 3 bajta, reg A uzima vrednost sa adrese addr
                    registri[A] = ram[addr];
        case 0x3b:  //DCX SP - SP--
                    (*stekPointer)++;
                    break;
        case 0x3f:  //CMC - negira carry bit
                    flags[CARRY] = !flags[CARRY];
                    break;

        case 0x76:  //HLT - zaustavlja procesor
                    (*programCounter)--;
                    break;

        case 0xc0:  //RNZ - ako je zero false vraca se na adresu upisanu na vrh steka
                    if(!flags[ZERO]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xc1:  //POP B - ucitava u reg C sa stekP i reg B sa stekP + 1, stekP += 2
                    registri[C] = ram[(*stekPointer)++];
                    registri[B] = ram[(*stekPointer)++];
                    break;
        case 0xc2:  //JNZ addr - 3 bajata, skace na addr ako je zero flag false
                    if(!flags[ZERO])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xc3:  //JMP addr
                    *programCounter = addr - 1;
                    break;
        case 0xc4:  //CNZ addr - 3 bajta ako je zero flag false onda CALL
                    (*programCounter) += 2;
                    if(!flags[ZERO]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xc5:  //PUSH B - cuva na adresi stekPointera - 1 register B i na -2 reg C
                    ram[--(*stekPointer)] = registri[B];
                    ram[--(*stekPointer)] = registri[C];
                    break;
        case 0xc7:  //RST 0 - skace na subrutinu na adresi 0 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = MAX_ADDRESS;
                    break;
        case 0xc8:  //RZ - ako je zero true vraca se na adresu upisanu na vrh steka
                    if(flags[ZERO]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xc9:  //RET - vraca se na adresu upisanu na vrh steka
                    *programCounter = ram[(*stekPointer)++];
                    (*programCounter) |= ram[(*stekPointer)++] << 8;
                    break;
        case 0xca:  //JZ addr - 3 bajta, skace na addr ako je zero flag true
                    if(flags[ZERO])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xcb:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0xcc:  //CZ addr - 3 bajta ako je zero flag true onda CALL
                    (*programCounter) += 2;
                    if(flags[ZERO]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xcd:  //CALL addr - 3 bajta, druga dva adresa na kaju se skace, trenutna se pamti na steku
                    (*programCounter) += 2;
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = addr - 1;
                    break;
        case 0xcf:  //RST 1 - skace na subrutinu na adresi 8 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 7;
                    break;
        case 0xd0:  //RNC - ako je carry false vraca se na adresu upisanu na vrh steka
                    if(!flags[CARRY]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xd1:  //POP D - ucitava u reg E sa stekP i reg D sa stekP + 1, stekP += 2
                    registri[E] = ram[(*stekPointer)++];
                    registri[D] = ram[(*stekPointer)++];
                    break;
        case 0xd2:  //JNC addr- 3 bajta, skace na addr ako je carry flag false
                    if(!flags[CARRY])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xd3:  //OUT data - 2 bajta salje izlazne podatke uredjaju na adresi data
        case 0xd4:  //CNC addr - 3 bajta ako je carry flag false onda CALL
                    (*programCounter) += 2;
                    if(!flags[CARRY]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xd5:  //PUSH D - cuva na adresi stekPointera - 1 register D i na -2 reg E
                    ram[--(*stekPointer)] = registri[D];
                    ram[--(*stekPointer)] = registri[E];
                    break;
        case 0xd7:  //RST 2 - skace na subrutinu na adresi 16 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 15;
                    break;
        case 0xd8:  //RC - ako je carry true vraca se na adresu upisanu na vrh steka
                    if(flags[CARRY]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xd9:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0xda:  //JC addr- 3 bajta, skace na addr ako je carry flag true
                    if(flags[CARRY])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xdb:  //IN data - 2 bajta prima ulazne podatke sa uredjaja na adresi data

        case 0xdc:  //CC addr - 3 bajta ako je carry flag true onda CALL
                    (*programCounter) += 2;
                    if(flags[CARRY]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xdd:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0xdf:  //RST 3 - skace na subrutinu na adresi 24 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 23;
                    break;
        case 0xe0:  //RPO - ako je parity false vraca se na adresu upisanu na vrh steka
                    if(!flags[PARITY]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xe1:  //POP H - ucitava u reg L sa stekP i reg H sa stekP + 1, stekP += 2
                    registri[L] = ram[(*stekPointer)++];
                    registri[H] = ram[(*stekPointer)++];
                    break;
        case 0xe2:  //JPO addr - 3 bajta, skace na addr ako je neparno, tj parity flag false
                    if(!flags[PARITY])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xe3:  //XTHL - menja vrednosti L i vrednosti u ramu na adresi stekPointera
                    // i H sa vrednoscu na adresi stekPointera + 1
                    temp = registri[L];
                    registri[L] = ram[(*stekPointer)];
                    ram[(*stekPointer)] = temp;
                    temp = registri[H];
                    registri[H] = ram[(*stekPointer) + 1];
                    ram[(*stekPointer) + 1] = temp;
                    break;
        case 0xe4:  //CPO addr - 3 bajta ako je parity flag false onda CALL
                    (*programCounter) += 2;
                    if(!flags[PARITY]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xe5:  //PUSH H - cuva na adresi stekPointera - 1 register H i na -2 reg L
                    ram[--(*stekPointer)] = registri[H];
                    ram[--(*stekPointer)] = registri[L];
                    break;
        case 0xe7:  //RST 4 - skace na subrutinu na adresi 32 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 31;
                    break;
        case 0xe8:  //RPE - ako je parity true vraca se na adresu upisanu na vrh steka
                    if(flags[PARITY]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xe9:  //PCHL - ucitava u brojac adresa adresu iz HL
                    *programCounter = (registri[L] | (registri[H] << 8)) - 1;
                    break;
        case 0xea:  //JPE - 3 bajta, skace na addr ako je parno, tj parity flag true
                    if(flags[PARITY])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xeb:  //XCHG menja vrednosti H i D kao i L i B
                    temp = registri[H];
                    registri[H] = registri[D];
                    registri[D] = temp;
                    temp = registri[L];
                    registri[L] = registri[E];
                    registri[E] = temp;
                    break;
        case 0xec:  //CPE - 3 bajta ako je parity flag true onda CALL
                    (*programCounter) += 2;
                    if(flags[PARITY]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xed:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0xef:  //RST 5 - skace na subrutinu na adresi 40 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 39;
                    break;
        case 0xf0:  //RP - ako je sign false vraca se na adresu upisanu na vrh steka
                    if(!flags[SIGN]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xf1:  //POP PSW - ucitava u SZ0A0P1C sa stekP i reg A sa stekP + 1, stekP += 2
                    psw = ram[(*stekPointer)++];
                    flags[CARRY] = psw & 0x1;
                    flags[PARITY] = (psw & 0x4) >> 2;
                    flags[AUX_CARRY] = (psw & 0x10) >> 4;
                    flags[ZERO] = (psw & 0x40) >> 6;
                    flags[SIGN] = (psw & 0x80) >> 7;
                    registri[A] = ram[(*stekPointer)++];
                    break;
        case 0xf2:  //JP addr - 3 bajta, skace na addr ako je sign flag false
                    if(!flags[SIGN])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xf3:  //DI - spreci interupts
                    flags[INTE] = false;
                    break;
        case 0xf4:  //CP addr - 3 bajta ako je sign flag false onda CALL
                    (*programCounter) += 2;
                    if(!flags[SIGN]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xf5:  //PUSH PSW - cuva na adresi stekPointera - 1 register A i na -2 reg flags
                    //flags - bitovi od 7. ka 1. SZ0A0P1C
                    ram[--(*stekPointer)] = registri[A];
                    ram[--(*stekPointer)] = (flags[SIGN] << 7) | (flags[ZERO] << 6) | (flags[AUX_CARRY] << 4) | (flags[PARITY] << 2) | 0x2 | flags[CARRY];
                    break;
        case 0xf7:  //RST 6 - skace na subrutinu na adresi 48 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 47;
                    break;
        case 0xf8:  //RM - ako je sign true vraca se na adresu upisanu na vrh steka
                    if(flags[SIGN]){
                        *programCounter = ram[(*stekPointer)++];
                        (*programCounter) |= ram[(*stekPointer)++] << 8;
                    }
                    break;
        case 0xf9:  //SPHL - inicijalizuje stek pointer adresom iz reg H i L
                    *stekPointer = (registri[H] << 8) | registri[L];
                    break;
        case 0xfa:  //JM addr - 3 bajta, skace na addr ako je sign flag true
                    if(flags[SIGN])
                        *programCounter = addr - 1;
                    else
                        *programCounter += 2;
                    break;
        case 0xfb:  //EI - omoguci interupts
                    flags[INTE] = true;
                    break;
        case 0xfc:  //CM - 3 bajta ako je sign flag true onda CALL
                    (*programCounter) += 2;
                    if(flags[SIGN]){
                        ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                        ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                        *programCounter = addr - 1;
                    }
                    break;
        case 0xfd:  //nepostojeca instrukcija
                    printf("Nepostojeca instrukcija\n");
                    exit(0);
        case 0xff:  //RST 7 - skace na subrutinu na adresi 56 kad se desi interept
                    ram[--(*stekPointer)] = ((*programCounter) & 0xFF00) >> 8;
                    ram[--(*stekPointer)] = (*programCounter) & 0xFF;
                    *programCounter = 55;
                    break;
        default:    nadjena = false;
    }
    if(nadjena){
        (*programCounter)++;
        return;
    }

    BYTE auxRez, vrednost, addCarry = 0;
    WORD rez;
    BYTE dest, source, destReg, sourceReg;
    WORD adresa = (registri[H] << 8) | registri[L];
    source = instrukcija & 0x7;
    dest = (instrukcija & 0x38) >> 3;
    //printf("%d\n\n", dest);
    sourceReg = source;
    if(source == 7)
        sourceReg = 6;
    if(source == 6)
        sourceReg = 8;
    destReg = dest;
    if(dest == 7)
        destReg = 6;
    if(dest == 6)
        destReg = 8;
    // source - poslednje tri cifre opkoda ovih instrukcija vrednost govori koji registar
    // se koristi, oznacava redni broj registra osim kada je 6 oznacava memoriju adrese u
    // registrima H i L i 7 oznacava reg A, dest je isto samo gde se pamti
    if((instrukcija >= 0x40) && (instrukcija < 0x80)){  // MOV - pomera podatke
        if(destReg == 8){
            ram[adresa] = registri[sourceReg];
        }else if(sourceReg == 8){
            registri[destReg] = ram[adresa];
        }else{
            registri[destReg] = registri[sourceReg];
        }
    }else if(((instrukcija >= 0x80) && (instrukcija < 0x90)) || (instrukcija == 0xc6) || (instrukcija == 0xce)){
        //ADD - dodaje na reg A, 0xc6 je ADI data - dvobajtovska instrukcija dodaje data na reg A
        //0xce - ACI data - dvobajtovska instrukcija dodaje carry i data na akumulator
        if(((instrukcija >= 0x88) && (instrukcija < 0x90)) || (instrukcija == 0xce))
            //ADC - dadaje sa prenosom na reg A, ili ACI
            addCarry = flags[CARRY];
        if((instrukcija == 0xc6) || (instrukcija == 0xce)){
            vrednost = ram[++(*programCounter)];
        }else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        auxRez = (vrednost & 0xf) + (registri[A] & 0xf) + addCarry;
        flags[AUX_CARRY] = setAuxCarry(auxRez);
        rez = vrednost + registri[A] + addCarry;
        flags[CARRY] = setCarry(rez);
        registri[A] = rez & 0xff;
        flags[ZERO] = setZero(registri[A]);
        flags[PARITY] = setParity(registri[A]);
        flags[SIGN] = setSign(registri[A]);
        //printf("%d\n\n", sourceReg);
    }else if(((instrukcija >= 0x90) && (instrukcija < 0xa0)) || (instrukcija == 0xd6) || (instrukcija == 0xde)){ //SUB - oduzima od A 90 - 98
        //SBB - oduzima sa borow 98 - a0, SUI data- dva bajta d6,
        //SBI data- dva bajta de
        if(((instrukcija >= 0x98) && (instrukcija < 0xa0)) || (instrukcija == 0xde))
            //SBB - dadaje sa prenosom na reg A, ili SBI
            addCarry = flags[CARRY];
        if((instrukcija == 0xd6) || (instrukcija == 0xde)){
            vrednost = ram[++(*programCounter)];
        }else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        vrednost = ~vrednost;
        auxRez = (vrednost & 0xf) + (registri[A] & 0xf) + addCarry + 1;
        flags[AUX_CARRY] = setAuxCarry(auxRez);
        rez = vrednost + registri[A] + addCarry + 1;
        flags[CARRY] = !setCarry(rez);
        registri[A] = rez & 0xff;
        flags[ZERO] = setZero(registri[A]);
        flags[PARITY] = setParity(registri[A]);
        flags[SIGN] = setSign(registri[A]);
    }else if(((instrukcija >= 0xa0) && (instrukcija < 0xa8)) || (instrukcija == 0xe6)){
        // ANA - AND sa A, 0xe6 - ANI data-dvobajtovska AND data sa A
        if(instrukcija == 0xe6)
            vrednost = ram[++(*programCounter)];
        else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        registri[A] &= vrednost;
        flags[CARRY] = false;
        flags[ZERO] = setZero(registri[A]);
        flags[PARITY] = setParity(registri[A]);
        flags[SIGN] = setSign(registri[A]);

    }else if(((instrukcija >= 0xa8) && (instrukcija < 0xb0)) || (instrukcija == 0xee)){
        // XRA - XOR sa A, 0xee - XRI data- XOR data sa A
        if(instrukcija == 0xee)
           vrednost = ram[++(*programCounter)];
        else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        registri[A] ^= vrednost;
        flags[CARRY] = false;
        flags[ZERO] = setZero(registri[A]);
        flags[PARITY] = setParity(registri[A]);
        flags[SIGN] = setSign(registri[A]);
        //AUX ?????????????

    }else if(((instrukcija >= 0xb0) && (instrukcija < 0xb8)) || (instrukcija == 0xee)){
        // ORA - OR sa A, 0xf6 - ORI data- OR data sa A
        if(instrukcija == 0xf6)
           vrednost = ram[++(*programCounter)];
        else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        registri[A] |= vrednost;
        flags[CARRY] = false;
        flags[ZERO] = setZero(registri[A]);
        flags[PARITY] = setParity(registri[A]);
        flags[SIGN] = setSign(registri[A]);

    }else if(((instrukcija >= 0xb8) && (instrukcija < 0xc0)) || (instrukcija == 0xfe)){ // CMP - poredi sa A
        //CPI data - dva bajta fe, poredi data sa A
        if(instrukcija == 0xfe)
           vrednost = ram[++(*programCounter)];
        else if(destReg == 8)
            vrednost = ram[adresa];
        else
            vrednost = registri[sourceReg];
        vrednost = ~vrednost;
        auxRez = (vrednost & 0xf) + (registri[A] & 0xf) + addCarry + 1;
        flags[AUX_CARRY] = setAuxCarry(auxRez);
        rez = vrednost + registri[A] + addCarry + 1;
        flags[CARRY] = !setCarry(rez);
        rez &= 0xff;
        flags[ZERO] = setZero(rez);
        flags[PARITY] = setParity(rez);
        flags[SIGN] = setSign(rez);


    }else if((instrukcija & 0xc7) == 0x6){  // MVI - dvobajtovska instrukcija unosi podatak na dest
        vrednost = ram[++(*programCounter)];
        if(destReg == 8)
            ram[adresa] = vrednost;
        else
            registri[destReg] = vrednost;
    }else if((instrukcija & 0xc7) == 0x4){  // INR dest(B, C ...)- dodaje jedan registru dest
        if(destReg == 8)
            vrednost = ++ram[adresa];
        else
            vrednost = ++registri[destReg];
        flags[ZERO] = setZero(vrednost);
        flags[PARITY] = setParity(vrednost);
        flags[SIGN] = setSign(vrednost);
        flags[AUX_CARRY] = ((vrednost & 0xf) == 0);
        //CARRY ??????????
    }else if((instrukcija & 0xc7) == 0x5){  // DCR dest(B, C ...)- oduzima jedan od reg dest
        if(destReg == 8)
            vrednost = --ram[adresa];
        else
            vrednost = --registri[destReg];
        flags[ZERO] = setZero(vrednost);
        flags[PARITY] = setParity(vrednost);
        flags[SIGN] = setSign(vrednost);
        //AUX ??????????????????
    }
    (*programCounter)++;
}

bool setParity(BYTE vrednost){
    int brojac = 0;
    BYTE mask = 1;
    for(int i = 0; i < 8; i++, mask *= 2)
        brojac += (vrednost & mask) >> i;
    if(brojac % 2)
        return false;
    return true;
}
bool setCarry(WORD vrednost){
    if(vrednost > 0xff)
        return true;
    return false;
}
bool setAuxCarry(BYTE vrednost){
    if(vrednost > 0xf)
        return true;
    return false;
}
bool setSign(BYTE vrednost){
    if(vrednost > 0x7f)
        return true;
    return false;
}
bool setZero(BYTE vrednost){
    if(vrednost == 0)
        return true;
    return false;
}

void showStats(BYTE* ram, BYTE* registri, WORD* stekPointer, WORD* programCounter, bool* flags, bool mode){
    //mode = true - ispisuje u kom su stanju delovi kompa
    //mode = false - ispisuje u kom su stanju promenjeni delovi kompa
    char oznaka;
    char vrstaZastave[7];
    BYTE ramC[MAX_ADDRESS];
    BYTE registriC[BROJ_REGISTARA];
    WORD stekPointerC;
    WORD programCounterC;
    bool flagsC[BROJ_ZASTAVA];
    if(!mode){
    memcpy(ramC, ram, MAX_ADDRESS);
    memcpy(registriC, registri, BROJ_REGISTARA);
    memcpy(flagsC, flags, BROJ_ZASTAVA * sizeof(bool));
    stekPointerC = *stekPointer;
    programCounterC = *programCounter;
    }
    exeInst(ram, registri, stekPointer, programCounter, flags);
    for(int i = 0; i < BROJ_REGISTARA; i++){
        if((mode) || (registri[i] != registriC[i])){
            switch(i){
                case 0: oznaka = 'B';
                        break;
                case 1: oznaka = 'C';
                        break;
                case 2: oznaka = 'D';
                        break;
                case 3: oznaka = 'E';
                        break;
                case 4: oznaka = 'H';
                        break;
                case 5: oznaka = 'L';
                        break;
                case 6: oznaka = 'A';
                        break;
            }
            if(mode)
                printf("Registar %c(%d) = %d\n", oznaka, i, registri[i]);
            else
                printf("Registar %c(%d) = %d (%d)\n", oznaka, i, registri[i], registriC[i]);
        }
    }
    for(int i = 0; i < BROJ_ZASTAVA; i++){
        if((mode) || (flags[i] != flagsC[i])){
            switch(i){
                case 0: strcpy(vrstaZastave, "CARRY");
                        break;
                case 1: strcpy(vrstaZastave, "AUX");
                        break;
                case 2: strcpy(vrstaZastave, "SIGN");
                        break;
                case 3: strcpy(vrstaZastave, "ZERO");
                        break;
                case 4: strcpy(vrstaZastave, "PARITY");
                        break;
            }
            if(mode)
                printf("Flag %s = %d\n", vrstaZastave, flags[i]);
            else
                printf("Flag %s = %d (%d)\n", vrstaZastave, flags[i], flagsC[i]);
        }
    }
    int adr = 10;
    if(!mode)
        adr = MAX_ADDRESS;
    for(int i = 0; i < adr; i++){
        if(mode)
            printf("Ram [%d] = %d\n", i, ram[i]);
        else if(ram[i] != ramC[i])
            printf("Ram [%d] = %d (%d)\n", i, ram[i], ramC[i]);
    }
    printf("Brojac programa = %d\n", *programCounter);
    if(mode){
        printf("Stek pointer = %d\n", *stekPointer);
    }else if(stekPointerC != *stekPointer){
        printf("Stek pointer = %d (%d)\n", *stekPointer, stekPointerC);
    }
    printf("-------------------------------------------\n");
}

WORD readMachineCode(char* imeFajla, BYTE* code){
    long long sizeF;
    FILE* fajl;
    fajl = fopen(imeFajla, "rb");
    if(fajl == NULL){
        printf("Nepostojeci fajl\n");
        exit(0);
    }
    fseek(fajl, 0, SEEK_END);
    sizeF = ftell(fajl);
    //printf("%d\n\n", sizeF);
    if(sizeF > MAX_ADDRESS){
        printf("Nema memorije\n");
        exit(0);
    }
    fseek(fajl, 0, SEEK_SET);
    fread(code, 1, sizeF, fajl);
    fclose(fajl);
    return sizeF;
}

void runProgramFromTemp(void){
    BYTE registri[BROJ_REGISTARA] = {0x0};  //8-bitni registri redom B, C, D, E, H, L, A
    WORD stekPointer = 0x0;     //16-bitni registar koji cuva adresu steka
    WORD programCounter = 0x0;  //16-bitni registar koji cuva adresu sledece instrukcije za izvrsavanje
    BYTE ram[MAX_ADDRESS] = {0x0};  //radna memorija sa 65536 adresa
    bool flags[BROJ_ZASTAVA] = {false};     // zastave redom CARRY, AUX_CARRY, SIGN, ZERO, PARITY
    WORD granica;
    printf("------------------------------SVABA8080------------------------------\n");
    printf("-----------------------------Verzija 1.1-----------------------------\n\n");
    granica = readMachineCode(".\\tempMachine.b", ram);
    //printf("%d\n", granica);
    for(int i = 0; i < 20; i++){
        showStats(ram, registri, &stekPointer, &programCounter, flags, false);
    }
}
