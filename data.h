#ifndef __DATA_H__
#define __DATA_H__

#define MAX_CODE_LENGTH 500
#define AR_VARIABLE_OFFSET 4

// Instruction
typedef struct {
    int op;  // opcode
    int r;   // reg
    int l;   // L
    int m;   // M
} Instruction;

// Opcodes
enum {
    LIT = 1, RTN = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8,

    SIO_WRITE = 9, SIO_READ = 10, SIO_HALT = 11,
    
    NEG = 12, ADD = 13, SUB = 14, MUL = 15, DIV = 16, ODD = 17, MOD = 18,
    EQL = 19, NEQ = 20, LSS = 21, LEQ = 22, GTR = 23, GEQ = 24
};

// Numerical values assigned to each token
enum {
    nulsym     =  1, identsym =  2, numbersym    =  3,

    // Special symbols
                                                       plussym   =  4, minussym   =  5, 
    multsym    =  6, slashsym =  7, oddsym       =  8, eqsym     =  9, neqsym     = 10, 
    lessym     = 11, leqsym   = 12, gtrsym       = 13, geqsym    = 14, lparentsym = 15,
    rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, becomessym = 20, 
    
    // Reserved words
    beginsym = 21, endsym  = 22, ifsym    = 23, thensym = 24, whilesym = 25, 
    dosym    = 26, callsym = 27, constsym = 28, varsym  = 29, procsym  = 30, 
    writesym = 31, readsym = 32, elsesym  = 33
};

// Enumeration for non-terminals
typedef enum {
    PROGRAM, BLOCK, CONST_DECLARATION, VAR_DECLARATION, PROC_DECLARATION,
    STATEMENT, CONDITION, REL_OP, EXPRESSION, TERM, FACTOR
} NonTerminal;

// The string representation of each token, if applicable (identsym and numbersym excluded)
extern const char* tokenNames[];

extern const char* codeGeneratorErrMsg[];

extern const char* nonTerminalNames[];

extern const char* opcodeNames[];

#endif