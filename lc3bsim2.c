/*
    Name 1: Julia Rebello
    Name 2: Clara Johnson
    UTEID 1: jlr3755
    UTEID 2: caj2773
*/

/* ************************************************************* */
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/* ************************************************************* */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ************************************************************* */
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/* ************************************************************* */

/* ************************************************************* */
/* These are the functions you'll have to write.               */
/* ************************************************************* */

void process_instruction();

/* ************************************************************* */
/* A couple of useful definitions.                             */
/* ************************************************************* */
#define FALSE 0
#define TRUE  1

/* ************************************************************* */
/* Use this to avoid overflowing 16 bits on the bus.           */
/* ************************************************************* */
#define Low16bits(x) ((x) & 0xFFFF)

/* ************************************************************* */
/* Main memory.                                                */
/* ************************************************************* */
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A
*/

#define WORDS_IN_MEM    0x08000
int MEMORY[WORDS_IN_MEM][2];

/* ************************************************************* */

/* ************************************************************* */

/* ************************************************************* */
/* LC-3b State info.                                           */
/* ************************************************************* */
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

    int PC,		/* program counter */
            N,		/* n condition bit */
            Z,		/* z condition bit */
            P;		/* p condition bit */
    int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* ************************************************************* */
/* A cycle counter.                                            */
/* ************************************************************* */
int INSTRUCTION_COUNT;

/* ************************************************************* */
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/* ************************************************************* */
void help() {
    printf("----------------LC-3b ISIM Help-----------------------\n");
    printf("go               -  run program to completion         \n");
    printf("run n            -  execute program for n instructions\n");
    printf("mdump low high   -  dump memory from low to high      \n");
    printf("rdump            -  dump the register & bus values    \n");
    printf("?                -  display this help menu            \n");
    printf("quit             -  exit the program                  \n\n");
}

/* ************************************************************* */
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/* ************************************************************* */
void cycle() {

    process_instruction();
    CURRENT_LATCHES = NEXT_LATCHES;
    INSTRUCTION_COUNT++;
}

/* ************************************************************* */
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/* ************************************************************* */
void run(int num_cycles) {
    int i;

    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (CURRENT_LATCHES.PC == 0x0000) {
            RUN_BIT = FALSE;
            printf("Simulator halted\n\n");
            break;
        }
        cycle();
    }
}

/* ************************************************************* */
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/* ************************************************************* */
void go() {
    if (RUN_BIT == FALSE) {
        printf("Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
        cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/* ************************************************************* */
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/* ************************************************************* */
void mdump(FILE * dumpsim_file, int start, int stop) {
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
        fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/* ************************************************************* */
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/* ************************************************************* */
void rdump(FILE * dumpsim_file) {
    int k;

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
    fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
        fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/* ************************************************************* */
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */
/*                                                             */
/* ************************************************************* */
void get_command(FILE * dumpsim_file) {
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
        case 'G':
        case 'g':
            go();
            break;

        case 'M':
        case 'm':
            scanf("%i %i", &start, &stop);
            mdump(dumpsim_file, start, stop);
            break;

        case '?':
            help();
            break;
        case 'Q':
        case 'q':
            printf("Bye.\n");
            exit(0);

        case 'R':
        case 'r':
            if (buffer[1] == 'd' || buffer[1] == 'D')
                rdump(dumpsim_file);
            else {
                scanf("%d", &cycles);
                run(cycles);
            }
            break;

        default:
            printf("Invalid Command\n");
            break;
    }
}

/* ************************************************************* */
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/* ************************************************************* */
void init_memory() {
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
        MEMORY[i][0] = 0;
        MEMORY[i][1] = 0;
    }
}

/* ************************************************************ */
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/* ************************************************************ */
void load_program(char *program_filename) {
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
        program_base = word >> 1;
    else {
        printf("Error: Program file is empty\n");
        exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
        /* Make sure it fits. */
        if (program_base + ii >= WORDS_IN_MEM) {
            printf("Error: Program file %s is too long to fit in memory. %x\n",
                   program_filename, ii);
            exit(-1);
        }

        /* Write the word to memory array. */
        MEMORY[program_base + ii][0] = word & 0x00FF;
        MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
        ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/* ********************************************************** */
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */
/*             and set up initial state of the machine.     */
/*                                                          */
/* ********************************************************** */
void initialize(char *program_filename, int num_prog_files) {
    int i;

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
        load_program(program_filename);
        while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/* ************************************************************* */
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/* ************************************************************* */
int main(int argc, char *argv[]) {
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 2) {
        printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
               argv[0]);
        exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argc - 1);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
        printf("Error: Can't open dumpsim file\n");
        exit(-1);
    }

    while (1)
        get_command(dumpsim_file);

}

/* ************************************************************* */
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.
   MEMORY
   CURRENT_LATCHES
   NEXT_LATCHES
   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.
   Begin your code here 	  			       */

/* ************************************************************* */

//OPCODE FUNCTIONS
//NOTE: we don't need RTI for this project.
void ADD(int num);
void AND(int num);
void BR(int num);
void JMP(int num);
void JSR(int num);
void LDB(int num);
void LDW(int num);
void LEA(int num);
void SHF(int num);
void STB(int num);
void STW(int num);
void TRAP(int num);
void XOR(int num);

//HELPER FUNCTIONS
void setBitArray(int num);
int powpow(int a, int b);
int regValue(int endBit, int startBit);
int signedValue(int endBit, int startBit);
void printHelper(char instr[]);

//bitArray Global Variable
int bitArray[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//                  15  14 13 12 11 10 9  8  7  6  5  4  3  2  1  0

void process_instruction(){
    /*  function: process_instruction
     *
     *    Process one instruction at a time
     *       -Fetch one instruction
     *       -Decode
     *       -Execute
     *       -Update NEXT_LATCHES
     */



        //FETCH the instruction
        int littleInstr = MEMORY[(CURRENT_LATCHES.PC)/2][0];
        int bigInstr = (MEMORY[(CURRENT_LATCHES.PC)/2][1]) << 8;
        int instr = bigInstr + littleInstr;

        //DECODE the instruction, then EXECUTE it using the OPCODE functions
        if ((instr || 0x1000) == 0x1000) {              //if ADD
            ADD(instr);

        } else if ((instr || 0x5000) == 0x5000) {       //if AND
            AND(instr);

        } else if ((instr || 0x9000) == 0x9000) {       //if XOR/NOT
            XOR(instr);

        } else if ((instr || 0xC000) == 0xC000) {       //if JMP/RET
            JMP(instr);

        } else if ((instr || 0x4000) == 0x4000) {       //if JSR/RR
            JSR(instr);

        } else if ((instr || 0x2000) == 0x2000) {       //if LDB
            LDB(instr);

        } else if ((instr || 0x6000) == 0x6000) {       //if LDW
            LDW(instr);

        } else if ((instr || 0xE000) == 0xE000) {       //if LEA
            LEA(instr);

        } else if ((instr || 0xD000) == 0xD000) {       //if SHF
            SHF(instr);

        } else if ((instr || 0x3000) == 0x3000) {       //if STB
            STB(instr);

        } else if ((instr || 0x7000) == 0x7000) {       //if STW
            STW(instr);

        } else if ((instr || 0xF000) == 0xF000) {       //if TRAP/HALT
            TRAP(instr);

        } else {       //if BR
            BR(instr);
        }
}

//***********************************************************************************************************************************
//************************************************ OPCODE FUNCTIONS *****************************************************************
//***********************************************************************************************************************************
void ADD(int num){
    setBitArray(num);
    int DR = regValue(11,9);                       //this is the destination register
    int SR1 = regValue(8,6);                       //this is the first source register

    //Update PC
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    //Execute the ADD
    if(bitArray[5]==0){                             //second source register mode
        int SR2 = regValue(2,0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
    }
    else{                                           //immediate operand mode
        int imm5 = signedValue(4,0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + imm5);
    }

    //Set the CCs
    if(NEXT_LATCHES.REGS[DR] == 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("ADD");
}

void AND(int num){
    setBitArray(num);
    int DR = regValue(11,9);                       //this is the destination register
    int SR1 = regValue(8,6);                       //this is the first source register

    //Update PC
    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    //Execute the AND
    if(bitArray[5]==0){                             //second source register mode
        int SR2 = regValue(2,0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & CURRENT_LATCHES.REGS[SR2]);
    }
    else{                                           //immediate operand mode
        int imm5 = signedValue(4,0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] & imm5);
    }

    //Set the CCs
    if(NEXT_LATCHES.REGS[DR] == 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("AND");
}


void BR(int num){
    setBitArray(num);
    int offset = signedValue(8,0);
    offset = offset << 1;

    if((bitArray[11] & CURRENT_LATCHES.N) | (bitArray[10] & CURRENT_LATCHES.Z) | (bitArray[9] & CURRENT_LATCHES.P)) {
        NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + offset;
    }

    NEXT_LATCHES.N = CURRENT_LATCHES.N;         //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("BRANCH");
}

void JMP(int num){
    setBitArray(num);
    int BR = regValue(8,6);                    //this is base register in either case
    NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BR];
    NEXT_LATCHES.N = CURRENT_LATCHES.N;         //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("JMP");
}

void JSR(int num){
    setBitArray(num);
    int TEMP = CURRENT_LATCHES.PC + 2;                  //TEMP gets the incremented PC (to match the Appendix A)

    if(bitArray[11] == 1){                                //JSR mode
        int offset = signedValue(10,0);
        offset = offset << 1;
        NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2 + offset;  //add offset to incremented PC
    }
    else{                                               //JSRR mode
        int BR = regValue(8,6);                        //base register
        NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[BR];
    }

    NEXT_LATCHES.REGS[7] = Low16bits(TEMP);

    NEXT_LATCHES.N = CURRENT_LATCHES.N;                 //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("JSR");
}

void LDB(int num){
    setBitArray(num);
    int DR = regValue(11,9);                           //destination register
    int BR = regValue(8,6);                            //base register
    int offset = signedValue(5,0);                        //offset
    int index = CURRENT_LATCHES.REGS[BR] + offset;

    if((index % 2) == 0) {
        NEXT_LATCHES.REGS[DR] = Low16bits(MEMORY[index/2][0]);
    } else {
        NEXT_LATCHES.REGS[DR] = Low16bits(MEMORY[index/2][1]);
    }

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;           //update PC
    if(NEXT_LATCHES.REGS[DR] == 0){                     //set condition codes
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("LDB");
}

void LDW(int num){
    setBitArray(num);
    int DR = regValue(11,9);                           //destination register
    int BR = regValue(8,6);                            //base register
    int offset = signedValue(5,0);                        //offset
    int TEMPlow, TEMPhigh;
    offset = offset << 1;

    TEMPlow = MEMORY[(CURRENT_LATCHES.REGS[BR]+offset)/2][0];
    TEMPhigh = MEMORY[(CURRENT_LATCHES.REGS[BR]+offset)/2][1];
    TEMPhigh = TEMPhigh << 8;

    NEXT_LATCHES.REGS[DR] = Low16bits(TEMPhigh + TEMPlow);

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;           //update PC
    if(NEXT_LATCHES.REGS[DR] == 0){                     //set condition codes
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("LDW");
}

void LEA(int num){
    setBitArray(num);
    int DR = regValue(11,9);                           //destination register
    int offset = signedValue(8,0);                        //offset

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;           //update PC

    NEXT_LATCHES.REGS[DR] = CURRENT_LATCHES.PC + 2 + offset;    //execute LEA

    NEXT_LATCHES.N = CURRENT_LATCHES.N;                 //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("LEA");
}

void SHF(int num){
    setBitArray(num);
    int DR = regValue(11, 9);
    int SR = regValue(8, 6);
    int amount4 = signedValue(3, 0);

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;

    if(bitArray[4] == 0) {
        NEXT_LATCHES.REGS[DR] = Low16bits((CURRENT_LATCHES.REGS[SR]) << amount4);
    } else {
        if(bitArray[5] == 0) {
            NEXT_LATCHES.REGS[DR] = Low16bits(((CURRENT_LATCHES.REGS[SR]) >> amount4) & !(0x00008000 >> (amount4 - 1)));
        } else {
            NEXT_LATCHES.REGS[DR] = Low16bits((CURRENT_LATCHES.REGS[SR]) >> amount4);
        }
    }

    if(NEXT_LATCHES.REGS[DR] == 0){                     //set condition codes
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("SHF");
}

void STB(int num){
    setBitArray(num);
    int SR = regValue(11, 9);
    int BaseR = regValue(8, 6);
    int offset = signedValue(5, 0);
    int index = CURRENT_LATCHES.REGS[BaseR] + offset;

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;           //update PC

    if((index % 2) == 0) {
        MEMORY[index/2][0] = CURRENT_LATCHES.REGS[SR];
    } else {
        MEMORY[index/2][1] = CURRENT_LATCHES.REGS[SR];
    }

    NEXT_LATCHES.N = CURRENT_LATCHES.N;                 //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("STB");
}

void STW(int num){
    setBitArray(num);
    int SR = regValue(11, 9);
    int BaseR = regValue(8, 6);
    int offset = signedValue(5, 0);
    offset = offset << 1;

    NEXT_LATCHES.PC = CURRENT_LATCHES.PC + 2;           //update PC

    MEMORY[(CURRENT_LATCHES.REGS[BaseR]+offset)/2][0] = CURRENT_LATCHES.REGS[SR] & 0xFF;
    MEMORY[(CURRENT_LATCHES.REGS[BaseR]+offset)/2][1] = (CURRENT_LATCHES.REGS[SR] >> 8) & 0xFF;

    NEXT_LATCHES.N = CURRENT_LATCHES.N;                 //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("STW");
}

void TRAP(int num){             //TRAP AND HALT
    int trapVector = regValue(7, 0);

    NEXT_LATCHES.REGS[7] = Low16bits(CURRENT_LATCHES.PC);
    NEXT_LATCHES.PC = MEMORY[(trapVector << 1)/2][0];

    NEXT_LATCHES.N = CURRENT_LATCHES.N;                 //update condition codes --no change from current
    NEXT_LATCHES.Z = CURRENT_LATCHES.Z;
    NEXT_LATCHES.P = CURRENT_LATCHES.P;

    printHelper("TRAP");
}

void XOR(int num){
    setBitArray(num);
    int DR = regValue(11, 9);
    int SR1 = regValue(8, 6);

    if(bitArray[5] == 0) {
        int SR2 = regValue(2, 0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ CURRENT_LATCHES.REGS[SR2]);
    } else {
        int imm5 = signedValue(4, 0);
        NEXT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] ^ imm5);
    }

    if(NEXT_LATCHES.REGS[DR] == 0){                     //set condition codes
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 1;
        NEXT_LATCHES.P = 0;
    }
    else if(NEXT_LATCHES.REGS[DR] > 0){
        NEXT_LATCHES.N = 0;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 1;

    }
    else if(NEXT_LATCHES.REGS[DR] < 0){
        NEXT_LATCHES.N = 1;
        NEXT_LATCHES.Z = 0;
        NEXT_LATCHES.P = 0;
    }

    printHelper("XOR");
}

//***********************************************************************************************************************************
//************************************************ HELPER FUNCTIONS *****************************************************************
//***********************************************************************************************************************************

void setBitArray(int num){
    for(int j = 15; j >= 0; j--){
        if(num >= powpow(2,j)){
            bitArray[j] = 1;
            num -= powpow(2,j);
        }
        else{
            bitArray[j] = 0;
        }
    }
}

int powpow(int a, int b){
    if(b == 0){return 1;}
    int res = a;
    for(int i = 0; i < b - 1; i++){
        res = res * a;
    }
    return res;
}

int regValue(int endBit, int startBit){
    int range = endBit - startBit;
    int res = 0;
    for(int i = 0; i <= range ; i++){
        if(bitArray[endBit-i] == 1){
            res += powpow(2,range-i);
        }
    }
    return res;
}

int signedValue(int endBit, int startBit) {
    int range = (endBit -1) - startBit;
    int res = 0;
    for(int i = 0; i <= range ; i++) {
        if (bitArray[(endBit - 1) - i] == 1) {
            res += powpow(2, range - i);
        }
    }

    if(bitArray[endBit] == 1) {
        res = res * -1;
    }

    return res;
}

void printHelper(char instr[]) {
    printf("INSTRUCTION: %s", instr);

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (int k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    printf("Next register/bus values :\n");
    printf("-------------------------------------\n");
    printf("PC                : 0x%.4x\n", NEXT_LATCHES.PC);
    printf("CCs: N = %d  Z = %d  P = %d\n", NEXT_LATCHES.N, NEXT_LATCHES.Z, NEXT_LATCHES.P);
    printf("Registers:\n");
    for (int k = 0; k < LC_3b_REGS; k++)
        printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");
    printf("\n");
    printf("\n");
}