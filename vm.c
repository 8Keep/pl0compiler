//Davis Rollman and Aashish Madamanchi

#include <stdio.h>
#include "vm.h"
#include "data.h"

/* ************************************************************************************ */
/* Declarations                                                                         */
/* ************************************************************************************ */

/**
 * Recommended design includes the following functions implemented.
 * However, you are free to change them as you wish inside the vm.c file.
 * */
void initVM(VirtualMachine*);

int readInstructions(FILE*, Instruction*);

void dumpInstructions(FILE*, Instruction*, int numOfIns);

int getBasePointer(int *stack, int currentBP, int L);

void dumpStack(FILE*, int* stack, int sp, int bp);

int executeInstruction(VirtualMachine* vm, Instruction ins, FILE* vmIn, FILE* vmOut);

/* ************************************************************************************ */
/* Global Data and misc structs & enums                                                 */
/* ************************************************************************************ */

/**
 * allows conversion from opcode to opcode string
 * */
const char *opcodes[] =
{
    "illegal", // opcode 0 is illegal
    "lit", "rtn", "lod", "sto", "cal", // 1, 2, 3 ..
    "inc", "jmp", "jpc", "sio", "sio",
    "sio", "neg", "add", "sub", "mul",
    "div", "odd", "mod", "eql", "neq",
    "lss", "leq", "gtr", "geq"
};

enum { CONT, HALT };

/* ************************************************************************************ */
/* Definitions                                                                          */
/* ************************************************************************************ */

/**
 * Initialize Virtual Machine
 * */
void initVM(VirtualMachine* vm)
{
    if(vm)
    {
        // Go ahead and set all the registers to zero
        vm->BP = 1;
        vm->SP = 0;
        vm->PC = 0;
        vm->IR = 0;
    }
}

/**
 * Fill the (ins)tructions array by reading instructions from (in)put file
 * Return the number of instructions read
 * */
int readInstructions(FILE* in, Instruction* ins)
{
    // Temp variables that will hold the data when we read the file
    int op, r, l, m, count = 0;

    // Loop through the file and read in opcode, reg, L, and M values and store into array
    while (1)
    {
        if (fscanf(in, "%d %d %d %d", &op, &r, &l, &m) == EOF)
        {
            break;
        }

        ins[count].op = op;
        ins[count].r = r;
        ins[count].l = l;
        ins[count].m  = m;
        count++;
    }
    return count;

}

/**
 * Dump instructions to the output file
 * */
void dumpInstructions(FILE* out, Instruction* ins, int numOfIns)
{
    int counter;
    // Header
    fprintf(out,
        "***Code Memory***\n%3s %3s %3s %3s %3s \n",
        "#", "OP", "R", "L", "M");

    for (counter = 0; counter < numOfIns; counter++)
    {
        fprintf(
            out,
            "%3d %3s %3d %3d %3d \n",
            counter,
            opcodes[ins[counter].op],
            ins[counter].r,
            ins[counter].l,
            ins[counter].m);
    }
    fprintf(out, "\n");
}

/**
 * Returns the base pointer for the lexiographic level L
 * */
int getBasePointer(int *stack, int currentBP, int L)
{
    int b1; //find base L levels down
    b1 = currentBP;
    while (L > 0)
    {
        b1 = stack[b1 + 1];
        L--;
    }
    return b1;
}

// Function that dumps the whole stack into output file
// Do not forget to use '|' character between stack frames
void dumpStack(FILE* out, int* stack, int sp, int bp)
{
    // Check if the base pointer is zero
    if(bp == 0)
      return;

    // bottom-most level, where a single zero value lies
      if(bp == 1)
      {
          fprintf(out, "%3d ", 0);
      }

      // former levels - if exists
      if(bp != 1)
      {
          dumpStack(out, stack, bp - 1, stack[bp + 2]);
      }

      // top level: current activation record
      if(bp <= sp)
      {
          // indicate a new activation record
          fprintf(out, "| ");

          // print the activation record
          int i;
          for(i = bp; i <= sp; i++)
          {
              fprintf(out, "%3d ", stack[i]);
          }
      }

}

/**
 * Executes the (ins)truction on the (v)irtual (m)achine.
 * This changes the state of the virtual machine.
 * Returns HALT if the executed instruction was meant to halt the VM.
 * .. Otherwise, returns CONT
 * */
int executeInstruction(VirtualMachine* vm, Instruction ins, FILE* vmIn, FILE* vmOut)
{
    switch(ins.op)
    {
        case 1:
            //LIT
            vm->RF[ins.r] = ins.m;
            break;
        case 2:
            //RTN
            vm->SP = vm->BP - 1;
            vm->BP = vm->stack[vm->SP + 3];
            vm->PC = vm->stack[vm->SP + 4];
            break;
        case 3:
            //LOD
            vm->RF[ins.r] = vm->stack[getBasePointer(vm->stack, vm->BP, ins.l) + ins.m];
            break;
        case 4:
            //STO
            vm->stack[getBasePointer(vm->stack, vm->BP, ins.l) + ins.m] = vm->RF[ins.r];
            break;
        case 5:
            //CAL
            vm->stack[vm->SP + 1] = 0;
            vm->stack[vm->SP + 2] = getBasePointer(vm->stack, vm->BP, ins.l);
            vm->stack[vm->SP + 3] = vm->BP;
            vm->stack[vm->SP + 4] = vm->PC;
            vm->BP = vm->SP + 1;
            vm->PC = ins.m;
            break;
        case 6:
            //INC
            vm->SP = vm->SP + ins.m;
            break;
        case 7:
            //JMP
            vm->PC = ins.m;
            break;
        case 8:
            //JPC
            if(vm->RF[ins.r] == 0)
            {
              vm->PC = ins.m;
            }
            break;
        case 9:
            //SIO
            if (ins.m == 1) {
                fprintf(vmOut, "%d ", vm->RF[ins.r]);
            }
            break;
        case 10:
            //SIO
            if (ins.m == 2) {
                fscanf(vmIn, "%d", &vm->RF[ins.r]);
            }
            break;
        case 11:
            //SIO
            if (ins.m == 3) {
                // Set halt flag to 1
                return HALT;
            }
            break;
        case 12:
            //NEG
            vm->RF[ins.r] = -(vm->RF[ins.l]);
            break;
        case 13:
            //ADD
            vm->RF[ins.r] = vm->RF[ins.l] + vm->RF[ins.m];
            break;
        case 14:
            //SUB
            vm->RF[ins.r] = vm->RF[ins.l] - vm->RF[ins.m];
            break;
        case 15:
            //MUL
            vm->RF[ins.r] = vm->RF[ins.l] * vm->RF[ins.m];
            break;
        case 16:
            //DIV
            vm->RF[ins.r] = vm->RF[ins.l] / vm->RF[ins.m];
            break;
        case 17:
            //ODD
            break;
        case 18:
            //MOD
            vm->RF[ins.r] = vm->RF[ins.l] % vm->RF[ins.m];
            break;
        case 19:
            //EQL
            if(vm->RF[ins.l] == vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        case 20:
            if(vm->RF[ins.l] != vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        case 21:
            //LSS
            if(vm->RF[ins.l] < vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        case 22:
            //LEQ
            if(vm->RF[ins.l] <= vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        case 23:
            //GTR
            if(vm->RF[ins.l]  > vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        case 24:
            //GEQ
            if(vm->RF[ins.l] >= vm->RF[ins.m])
            {
              vm->RF[ins.r] = 1;
            }
            else{
              vm->RF[ins.r] = 0;
            }
            break;
        default:
            fprintf(stderr, "Illegal instruction?");
            return HALT;
    }
    return CONT;
}

/**
 * inp: The FILE pointer containing the list of instructions to
 *         be loaded to code memory of the virtual machine.
 *
 * outp: The FILE pointer to write the simulation output, which
 *       contains both code memory and execution history.
 *
 * vm_inp: The FILE pointer that is going to be attached as the input
 *         stream to the virtual machine. Useful to feed input for SIO
 *         instructions.
 *
 * vm_outp: The FILE pointer that is going to be attached as the output
 *          stream to the virtual machine. Useful to save the output printed
 *          by SIO instructions.
 * */
void simulateVM(
    FILE* inp,
    FILE* outp,
    FILE* vm_inp,
    FILE* vm_outp
    )
{
    // Read instructions from file
    Instruction instr[MAX_CODE_LENGTH];
    int numInstr = 0;
    numInstr = readInstructions(inp, instr);
    
    // Dump instructions to the output file
    dumpInstructions(outp, instr, numInstr);

    // Before starting the code execution on the virtual machine,
    // .. write the header for the simulation part (***Execution***)
    fprintf(
        outp,
        "***Execution***\n%3s %3s %3s %3s %3s %3s %3s %3s %3s ", "#", "OP", "R", "L", "M", "PC", "BP", "SP", "STK");

    // Create a virtual machine
    VirtualMachine vm;

    // Initialize the virtual machine
    initVM(&vm);

    // Fetch&Execute the instructions on the virtual machine until halting
    int halt = 0;
    while(halt == CONT)
    {
        if (vm.PC > numInstr)
        {
            printf("halt");
            fflush(stdout);
            halt = HALT;
            break;
        }
        //Fetch
        vm.IR = vm.PC;
        vm.PC++;
        //Execute
        halt = executeInstruction(&vm, instr[vm.IR], vm_inp, vm_outp);

        fprintf(
        outp,
        "\n%3d %3s %3d %3d %3d %3d %3d %3d ", vm.IR, opcodes[instr[vm.IR].op], instr[vm.IR].r, instr[vm.IR].l, instr[vm.IR].m, vm.PC, vm.BP, vm.SP);
        dumpStack(outp, vm.stack, vm.SP, vm.BP);
    }

    // Above loop ends when machine halts. Therefore, dump halt message.
    fprintf(outp, "\nHLT\n");
    return;
}
