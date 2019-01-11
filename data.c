#include "data.h"

const char* tokenNames[] = {
    [nulsym] = "nulsym",

    [identsym] = "identsym",
    [numbersym] = "numbersym",

    // Special symbols (+ odd)
    [plussym]    = "plussym", [minussym] = "minussym", [multsym]      =  "multsym", [slashsym]    = "slashsym",
    [oddsym]     = "oddsym", [eqsym]    = "eqsym", [neqsym]       = "neqsym", [lessym]      = "lessym",
    [leqsym]     = "leqsym", [gtrsym]   = "gtrsym", [geqsym]       = "geqsym", [lparentsym]  = "lparentsym",
    [rparentsym] = "rparentsym", [commasym] = "commasym", [semicolonsym] = "semicolonsym",  [periodsym]   = "periodsym",
    [becomessym] = "becomessym", 

    // Reserved words
    [beginsym] = "beginsym", [endsym]  =  "endsym", [ifsym]   =        "ifsym", [thensym] = "thensym",
    [whilesym] = "whilesym", [dosym]   =   "dosym", [callsym] =      "callsym", 
    [constsym] = "constsym", [varsym]  =  "varsym", [procsym] = "procsym", 
    [writesym] = "writesym", [readsym] = "readsym", [elsesym] =      "elsesym"
};

const char* codeGeneratorErrMsg[] =
{
    [0] = "SUCCESS",
    [1] = "'=' must be followed by a number",
    [2] = "Identifier must be followed by '='",
    [3] = "'const', 'var', 'procedure', 'read', 'write' must be followed by identifier",
    [4] = "Semicolon or comma missing",
    [5] = "Semicolon missing",
    [6] = "Period expected",
    [7] = "Assignment operator expected",
    [8] = "'call' must be followed by an identifier",
    [9] = "'then' expected",
    [10] = "Semicolon or 'end' expected",
    [11] = "'do' expected",
    [12] = "Relational operator expected",
    [13] = "Right parenthesis missing",
    [14] = "The preceding factor cannot begin with this symbol",
    /* New error codes start below */
    [15] = "Identifier is undeclared or out of scope",
    [16] = "Assignment to constant or procedure is not allowed",
    [17] = "Call of a constant or variable is not allowed",
    [18] = "Write of a prodecure is not allowed",
    [19] = "Read to a constant or prodecure is not allowed"
};

const char* nonTerminalNames[] = {
    [PROGRAM] = "PROGRAM",
    [BLOCK] = "BLOCK",
    [CONST_DECLARATION] = "CONST_DECLARATION",
    [VAR_DECLARATION] = "VAR_DECLARATION",
    [PROC_DECLARATION] = "PROC_DECLARATION",
    [STATEMENT] = "STATEMENT",
    [CONDITION] = "CONDITION",
    [REL_OP] = "REL_OP",
    [EXPRESSION] = "EXPRESSION",
    [TERM] = "TERM",
    [FACTOR] = "FACTOR"
};

const char* opcodeNames[] = {
    [LIT] = "LIT", [RTN] = "RTN", [LOD] = "LOD", [STO] = "STO", [CAL] = "CAL", 
    [INC] = "INC", [JMP] = "JMP", [JPC] = "JPC",

    [SIO_WRITE] = "SIO_WRITE", [SIO_READ] = "SIO_READ", [SIO_HALT] = "SIO_HALT",
    
    [NEG] = "NEG", [ADD] = "ADD", [SUB] = "SUB", [MUL] = "MUL", [DIV] = "DIV",
    [ODD] = "ODD", [MOD] = "MOD", [EQL] = "EQL", [NEQ] = "NEQ", [LSS] = "LSS",
    [LEQ] = "LEQ", [GTR] = "GTR", [GEQ] = "GEQ"
};