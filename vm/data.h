#ifndef __DATA_H__
#define __DATA_H__

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH  500
#define MAX_LEXI_LEVELS  3
#define REGISTER_FILE_REG_COUNT 16

typedef struct {
    int op;  // opcode
    int r;   // reg
    int l;   // L
    int m;   // M
} Instruction;

/**
 * Virtual machine state holder
 * */
typedef struct VirtualMachine {
    /**
     * registers:
     *  BP: base pointer,
     *  SP: stack pointer,
     *  PC: program counter,
     *  IR: instruction register
     * */
    int BP, SP, PC, IR;

    /**
     * register file
     * */
    int RF[REGISTER_FILE_REG_COUNT];

    /**
     * stack
     * */
    int stack[MAX_STACK_HEIGHT];
} VirtualMachine;

#endif