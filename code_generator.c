#include "token.h"
#include "data.h"
#include "symbol.h"
#include <string.h>
#include <stdlib.h>

/**
 * This pointer is set when by codeGenerator() func and used by printEmittedCode() func.
 * 
 * You are not required to use it anywhere. The implemented part of the skeleton
 * handles the printing. Instead, you are required to fill the vmCode properly by making
 * use of emit() func.
 * */
FILE* _out;

/**
 * Token list iterator used by the code generator. It will be set once entered to
 * codeGenerator() and reset before exiting codeGenerator().
 * 
 * It is better to use the given helper functions to make use of token list iterator.
 * */
TokenListIterator _token_list_it;

/**
 * Current level. Use this to keep track of the current level for the symbol table entries.
 * */
unsigned int currentLevel;

/**
 * Current scope. Use this to keep track of the current scope for the symbol table entries.
 * NULL means global scope.
 * */
Symbol* currentScope;

/**
 * Symbol table.
 * */
SymbolTable symbolTable;

/**
 * The array of instructions that the generated(emitted) code will be held.
 * */
Instruction vmCode[MAX_CODE_LENGTH];

/**
 * The next index in the array of instructions (vmCode) to be filled.
 * */
int nextCodeIndex;

/**
 * The id of the register currently being used.
 * */
int currentReg;

/**
 * Emits the instruction whose fields are given as parameters.
 * Internally, writes the instruction to vmCode[nextCodeIndex] and returns the
 * nextCodeIndex by post-incrementing it.
 * If MAX_CODE_LENGTH is reached, prints an error message on stderr and exits.
 * */
int emit(int OP, int R, int L, int M);

/**
 * Prints the emitted code array (vmCode) to output file.
 * 
 * This func is called in the given codeGenerator() function. You are not required
 * to have another call to this function in your code.
 * */
void printEmittedCodes();

/**
 * Returns the current token using the token list iterator.
 * If it is the end of tokens, returns token with id nulsym.
 * */
Token getCurrentToken();

/**
 * Returns the type of the current token. Returns nulsym if it is the end of tokens.
 * */
int getCurrentTokenType();

/**
 * Advances the position of TokenListIterator by incrementing the current token
 * index by one.
 * */
void nextToken();

/**
 * Functions used for non-terminals of the grammar
 * 
 * rel-op func is removed on purpose. For code generation, it is easier to parse
 * rel-op as a part of condition.
 * */
int program();
int block();
int const_declaration();
int var_declaration();
int proc_declaration();
int statement(int reg);
int condition(int reg);
int expression(int reg);
int term(int reg);
int factor(int reg);

/******************************************************************************/
/* Definitions of helper functions starts *************************************/
/******************************************************************************/

Token getCurrentToken()
{
    return getCurrentTokenFromIterator(_token_list_it);
}

int getCurrentTokenType()
{
    return getCurrentToken().id;
}

void nextToken()
{
    _token_list_it.currentTokenInd++;
}

/**
 * Given the code generator error code, prints error message on file by applying
 * required formatting.
 * */
void printCGErr(int errCode, FILE* fp)
{
    if(!fp || !errCode) return;

    fprintf(fp, "CODE GENERATOR ERROR[%d]: %s.\n", errCode, codeGeneratorErrMsg[errCode]);
}

int emit(int OP, int R, int L, int M)
{
    if(nextCodeIndex == MAX_CODE_LENGTH)
    {
        fprintf(stderr, "MAX_CODE_LENGTH(%d) reached. Emit is unsuccessful: terminating code generator..\n", MAX_CODE_LENGTH);
        exit(0);
    }
    
    vmCode[nextCodeIndex] = (Instruction){ .op = OP, .r = R, .l = L, .m = M};    

    return nextCodeIndex++;
}

void printEmittedCodes()
{
    for(int i = 0; i < nextCodeIndex; i++)
    {
        Instruction c = vmCode[i];
        fprintf(_out, "%d %d %d %d\n", c.op, c.r, c.l, c.m);
    }
}

/******************************************************************************/
/* Definitions of helper functions ends ***************************************/
/******************************************************************************/

/**
 * Advertised codeGenerator function. Given token list, which is possibly the
 * output of the lexer, parses a program out of tokens and generates code. 
 * If encountered, returns the error code.
 * 
 * Returning 0 signals successful code generation.
 * Otherwise, returns a non-zero code generator error code.
 * */
int codeGenerator(TokenList tokenList, FILE* out)
{
    // Set output file pointer
    _out = out;

    /**
     * Create a token list iterator, which helps to keep track of the current
     * token being parsed.
     * */
    _token_list_it = getTokenListIterator(&tokenList);

    // Initialize current level to 0, which is the global level
    currentLevel = -1;

    // Initialize current scope to NULL, which is the global scope
    currentScope = NULL;

    // The index on the vmCode array that the next emitted code will be written
    nextCodeIndex = 0;

    // The id of the register currently being used
    currentReg = 0;

    // Initialize symbol table
    initSymbolTable(&symbolTable);

    // Start parsing by parsing program as the grammar suggests.
    int err = program();

    // Print symbol table - if no error occured
    if(!err)
    {
        // Print the emitted codes to the file
        printEmittedCodes();
    }

    // Reset output file pointer
    _out = NULL;

    // Reset the global TokenListIterator
    _token_list_it.currentTokenInd = 0;
    _token_list_it.tokenList = NULL;

    // Delete symbol table
    deleteSymbolTable(&symbolTable);

    // Return err code - which is 0 if parsing was successful
    return err;
}

// Already implemented.
int program()
{
    // Generate code for block
    int err = block();
    if(err) return err;

    // After parsing block, periodsym should show up
    if( getCurrentTokenType() == periodsym )
    {
        // Consume token
        nextToken();

        // End of program, emit halt code
        emit(SIO_HALT, 0, 0, 3);

        return 0;
    }
    else
    {
        // Periodsym was expected. Return error code 6.
        return 6;
    }
}

int block()
{
    currentLevel++;
    
    emit(INC, 0, 0, 4);
    
    int err = const_declaration();
    if (err)
        return err;

    err = var_declaration();
    if (err)
        return err;
    
    int instr = nextCodeIndex;
    emit(JMP, 0, 0, 0);
    
    err = proc_declaration();
    if (err)
        return err;
    
    //modify that jmp instr M to be this nextCodeIndex
    vmCode[instr].m = nextCodeIndex;
    
    err = statement(0);
    if (err)
        return err;
    
    //only return if current scope is not null, i.e. not global. u cant return from global scope. the program() will do halt instead.
    //if (currentScope)
        emit(RTN, 0, 0, 0);
    
    currentLevel--;

    return 0;
}

int const_declaration()
{

    if(getCurrentTokenType() == constsym)
    {
        // Loop until token !- commasym
        do
        {
            Symbol sym;
            sym.type = CONST;
            
            // Consume token and move on
            nextToken();

            // Check if the current token is not an identsym
            if(getCurrentTokenType() != identsym)
            {
                // Error: must have identifier after
                return 3;
            }
            
            // Copy the name into symbol table and consume the token
            strcpy(sym.name, getCurrentToken().lexeme);
            nextToken();
            
            // Check if token is equal symbol
            if(getCurrentTokenType() != eqsym)
            {
                return 2;
            }
            
            // Consume token and move on
            nextToken();
            
            if(getCurrentTokenType() != numbersym)
            {
                // Error: must have a number after
                return 1;
            }
            
            // Update symbol table value, level, and scope (which is currentScope)
            sym.value = atoi(getCurrentToken().lexeme);
            sym.level = currentLevel;
            sym.scope = currentScope;
            addSymbol(&symbolTable, sym);

            // Consume token and move onto next one
            nextToken();
        }
        while(getCurrentTokenType() == commasym);

        // Current token is not equal to semicolon
        if(getCurrentTokenType() != semicolonsym)
        {
            // Missing semicolon return error code 4
            return 4;
        }
        //printCurrentToken();
        nextToken();
    }

    // Successful parsing.
    return 0;
}

int var_declaration()
{
    
    if(getCurrentTokenType() == varsym)
    {
        // Count variable to account for activation record's contents (indexed at 0)
        int count = 0;

        // Loop until token !- commasym
        do
        {
            count++;

            // Symbol operations: type is set to VAR
            // address is updated to get a value of 4 since activation record has 4
            // scope is set to current scope
            Symbol sym;
            sym.type = VAR;
            sym.address = count + 3;
            sym.scope = currentScope;

            // Consume the token and move on
            nextToken();
            
            if(getCurrentTokenType() != identsym)
            {
                return 3;
            }
            
            strcpy(sym.name, getCurrentToken().lexeme);
            sym.level = currentLevel;
            addSymbol(&symbolTable, sym);
            
            //printCurrentToken();
            nextToken();
        }
        while(getCurrentTokenType() == commasym);


        if(getCurrentTokenType() != semicolonsym)
        {
            // Missing semicolon return error code 4
            return 4;
        }
        
        // Emit an increment with the offset being count (which was incremented in the loop to account for activation record) 
        emit(INC, 0, 0, count);

        // Consume the token and move onto next 
        nextToken();
    }   

    // Successful parsing.
    return 0;
}

int proc_declaration()
{
    while (getCurrentTokenType() == procsym)
    {
        Symbol sym;
        sym.type = PROC;
        
        nextToken();
        if (getCurrentTokenType() != identsym)
        {
            return 3;
        }
        
        strcpy(sym.name, getCurrentToken().lexeme);
        sym.level = currentLevel;
        sym.scope = currentScope;
        sym.address = nextCodeIndex;
        Symbol *tmpScope = currentScope;
        currentScope = addSymbol(&symbolTable, sym);
        nextToken();
        if (getCurrentTokenType() != semicolonsym)
        {
            return 5;
        }
        
        nextToken();
        
        int err = block();
        
        if (err)
            return err;

        if (getCurrentTokenType() != semicolonsym)
        {
            return 5;
        }
        nextToken();
    }
    return 0;
}

int statement(int reg)
{
    if(getCurrentTokenType() == identsym)
    {
        // Check for valid variable
        Symbol *sym = findSymbol(&symbolTable, currentScope, getCurrentToken().lexeme);
        
        // If error found then return undeclared identifier error
        if(!sym)
            return 15; 
        
        // Check if symboltype != variable
        if(sym->type != VAR)
        {
            // Error: Assignment to constant/procedure is not allowed
            return 16;
        }

        // Move onto next token
        nextToken();

        if(getCurrentTokenType() != becomessym)
        {
            // Return an error if its not becomessym
            return 7;
        }

        // Consume token
        nextToken();

        // Call EXPRESSION
        int err = expression(reg);

        if(err)
          return err;

        // Store the variable into a register
        emit(STO, reg, currentLevel - sym->level, sym->address);

        // Successful parsing
        return 0;
    }

    if(getCurrentTokenType() == callsym)
    {
        // Consume token
        nextToken();

        if(getCurrentTokenType() != identsym)
        {
            // Throw an error if its not an identsym after a callsym
            return 8;
        }
        Symbol *sym = findSymbol(&symbolTable, currentScope, getCurrentToken().lexeme);
        // If error found then return undeclared identifier error
        if(!sym)
            return 15;
        // Check if symboltype != proc
        if(sym->type != PROC)
        {
            // Error: Assignment to constant/procedure is not allowed
            return 17;
        }
        emit(CAL, 0, currentLevel - sym->level, sym->address); //TODO: Do we need to do currentLevel - sym->level for this one?
        // Get token
        nextToken();
        return 0;
    }

    if(getCurrentTokenType() == beginsym)
    {
        nextToken();

        int err = statement(reg);

        if(err)
          return err;

        while(getCurrentTokenType() == semicolonsym)
        {
            nextToken();

            err = statement(reg);
            if(err)
                return err;
        }

        if(getCurrentTokenType() != endsym)
        {
          return 10;
        }

        nextToken();

        return 0;
    }

    if(getCurrentTokenType() == ifsym)
    {
        // Consume the token and move forward
        nextToken();

        // Error check for condition
        int err = condition(reg);
        if(err)
            return err;

        // Check for then symbol
        if(getCurrentTokenType() != thensym)
        {
            // Then expected but not found so return error
            return 9;
        }
        
        // Hold onto the next index used in the VMCode array
        int instr = nextCodeIndex;

        // Jump conditionally when you skip to end of the "then" part of an if-then statement
        emit(JPC, reg, 0, 0);

        // Consume the token and move forward
        nextToken();

        // Error check for statement
        err = statement(reg);
        if(err)
            return err;

        // Update vmCode array 
        vmCode[instr].m = nextCodeIndex;
        
        if(getCurrentTokenType() == elsesym)
        {
            nextToken();
            vmCode[instr].m = nextCodeIndex + 1;
            instr = nextCodeIndex;
            emit(JMP, 0, 0, 0);
            
            err = statement(reg);
            if(err)
                return err;
            vmCode[instr].m = nextCodeIndex;
        }

        return 0;
    }

    if(getCurrentTokenType() == whilesym)
    {
        int instr1 = nextCodeIndex;
        nextToken();

        int err = condition(reg);
        if(err)
          return err;
        
        int instr2 = nextCodeIndex;
        emit(JPC, reg, 0, 0);

        if(getCurrentTokenType() != dosym)
        {
          // Do expected but not found return error
          return 11;
        }
        nextToken();

        err = statement(reg);
        if(err)
          return err;
        
        emit(JMP, 0, 0, instr1);
        vmCode[instr2].m = nextCodeIndex;

        return 0;
    }

    if(getCurrentTokenType() == readsym)
    {
        // Grab the token 
        nextToken();

        if(getCurrentTokenType() != identsym)
        {
            // Throw an error if identsym not found after readsym
            return 3;
        }

        // Grab the symbol you are on right now
        Symbol *sym = findSymbol(&symbolTable, currentScope, getCurrentToken().lexeme);
        
        if(sym->type != VAR)
        {
            // Cannot do assignment to constant or procedure
            return 16;
        }

        // Read the variable and then Store the variable
        emit(SIO_READ, reg, 0, 2);
        emit(STO, reg, currentLevel - sym->level, sym->address);

        // Get token
        nextToken();

        return 0;
    }

    if(getCurrentTokenType() == writesym)
    {
        // Move onto next token
        nextToken();

        if(getCurrentTokenType() != identsym)
        {
            // No ident after write throw an error
            return 3;
        }

        Symbol *sym = findSymbol(&symbolTable, currentScope, getCurrentToken().lexeme);

        // Check the symbol type to see if its a VAR. If so emit a LOD operation
        if(sym->type == VAR)
        {
            emit(LOD, reg, currentLevel - sym->level, sym->address);
        }

        // Check if symbol type is CONST. If so emit a LIT operation  
        else if(sym->type == CONST)
        {
            emit(LIT, reg, 0, sym->value);
        }
        else{
            // Error: Can't write a procedure
            return 18;
        }

        // Actual emit for writing
        emit(SIO_WRITE, reg, 0, 1);

        // Consume the token and move on
        nextToken();

        // Succesful parsing
        return 0;
    }

    // Successful parsing
    return 0;
}

int condition(int reg)
{
    if(getCurrentTokenType() == oddsym)
    {
        // If oddsym consume this token and move onto next one
        nextToken();

        // Call the expression and check if an error occured in parsing
        int err = expression(reg);
        // Throw out the error if it occurs
        if(err)
            return err;
        emit(ODD, reg, 0, 0);
    }
    else
    {
        // Call expression again
        int err = expression(reg);
        if (err)
            return err;

        int op = 0;
        
        //EQL = 19, NEQ = 20, LSS = 21, LEQ = 22, GTR = 23, GEQ = 24
        
        if (getCurrentTokenType() == eqsym)
        {
            op = EQL;
        }
        else if (getCurrentTokenType() == neqsym)
        {
            op = NEQ;
        }
        else if (getCurrentTokenType() == lessym)
        {
            op = LSS;
        }
        else if (getCurrentTokenType() == leqsym)
        {
            op = LEQ;
        }
        else if (getCurrentTokenType() == gtrsym)
        {
            op = GTR;
        }
        else if (getCurrentTokenType() == geqsym)
        {
            op = GEQ;
        }
        else
        {
            return 12; //relational operator expected
        }
        nextToken();
        err = expression(reg + 1);
        if (err)
            return err;
        
        emit(op, reg, reg, reg + 1);
    }

    // Successful parse
    return 0;
}

int expression(int reg)
{
    int op = 0;

    if(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym)
    {
        op = getCurrentTokenType();

        // Consume the token if its plussym or minussym
        nextToken();
    }

    int err = term(reg);
    
    if (op == minussym)
        emit(NEG, reg, reg, 0);

    if(err)
        return err;

    while(getCurrentTokenType() == plussym || getCurrentTokenType() == minussym)
    {
        op = getCurrentTokenType();
        nextToken();
        
        err = term(reg + 1);
        if(err)
            return err;
        
        emit(op == plussym ? ADD : SUB, reg, reg, reg + 1);
    }
    
    return 0;
}

int term(int reg)
{
    int err = factor(reg);

    if (err)
        return err;

    while(getCurrentTokenType() == multsym || getCurrentTokenType() == slashsym)
    {
        int tok = getCurrentTokenType();

        // Consume the token and move it forward
        nextToken();

        if(getCurrentToken().id == nulsym)
            return 6; // Error: Period expected

        // Call the factor function
        int fact = factor(reg + 1);

        // Check if the factor function passes it
        if(fact)
            return fact;
        
        // Emit either mult op or div op (reg = reg + (reg + 1))
        emit(tok == multsym ? MUL : DIV, reg, reg, reg + 1);
    }
    
    // Successful parsing
    return 0;
}

int factor(int reg)
{
    /**
     * There are three possibilities for factor:
     * 1) ident
     * 2) number
     * 3) '(' expression ')'
     * */

    // Is the current token a identsym?
    if(getCurrentTokenType() == identsym)
    {
        Symbol* sym = findSymbol(&symbolTable, currentScope, getCurrentToken().lexeme);
        if (!sym)
            return 15; // Error: identifier out of scope
        if (sym->type == VAR)
            emit(LOD, reg, currentLevel - sym->level, sym->address);
        else if (sym->type == CONST)
            emit(LIT, reg, 0, sym->value);
        else
            return 16;

        // Consume identsym
        nextToken(); // Go to the next token..

        // Success
        return 0;
    }

    // Is that a numbersym?
    else if(getCurrentTokenType() == numbersym)
    {
        int num = atoi(getCurrentToken().lexeme);
        emit(LIT, reg, 0, num);

        // Consume numbersym and move token forward
        nextToken(); 

        // Success
        return 0;
    }

    // Is that a lparentsym?
    else if(getCurrentTokenType() == lparentsym)
    {
        // Consume lparentsym and move to the next token
        nextToken(); 

        // Continue by parsing expression.
        int err = expression(reg);

        if(err) 
            return err;

        // After expression, right-parenthesis should come
        if(getCurrentTokenType() != rparentsym)
        {
            /**
             * Error code 13: Right parenthesis missing.
             * Stop parsing and return error code 13.
             * */
            return 13;
        }

        // It was a rparentsym. Consume rparentsym and move to next token.
        nextToken(); 
    }
    else
    {
        // Error code 14: The preceding factor cannot begin with this symbol.
        return 14;
    }

    return 0;
}
