#include "lexical_analyzer.h"
#include "data.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Declares isalpa, isdigit, isalnum

/* ************************************************************************** */
/* Enumarations, Typename Aliases, Helpers Structs ************************** */
/* ************************************************************************** */

typedef enum {
    ALPHA,   // a, b, .. , z, A, B, .. Z
    DIGIT, // 0, 1, .. , 9
    SPECIAL, // '>', '=', , .. , ';', ':'
    INVALID  // Invalid symbol
} SymbolType;

/**
 * Following struct is recommended to use to keep track of the current state
 * .. of the lexer, and modify the state in other functions by passing pointer
 * .. to the state as argument.
 * */
typedef struct {
    int lineNum;         // the line number currently being processed
    int charInd;         // the index of the character currently being processed
    char* sourceCode;    // null-terminated source code string
    LexErr lexerError;   // LexErr to be filled when Lexer faces an error
    TokenList tokenList; // list of tokens
} LexerState;

/* ************************************************************************** */
/* Declarations ************************************************************* */
/* ************************************************************************** */

/**
 * Initializes the LexerState with the given null-terminated source code string.
 * Sets the other fields of the LexerState to their inital values.
 * Shallow copying is done for the source code field.
 * */
void initLexerState(LexerState*, char* sourceCode);

/**
 * Returns 1 if the given character is valid.
 * Returns 0 otherwise.
 * */
int isCharacterValid(char);

/**
 * Returns 1 if the given character is one of the special symbols of PL/0,
 * .. such as '/', '=', ':' or ';'.
 * Returns 0 otherwise.
 * */
int isSpecialSymbol(char);

/**
 * Returns the symbol type of the given character.
 * */
SymbolType getSymbolType(char);

/**
 * Checks if the given symbol is one of the reserved token.
 * If yes, returns the numerical value assigned to the corresponding token.
 * If not, returns -1.
 * For example, calling the function with symbol "const" returns 28.
 * */
int checkReservedTokens(char* symbol);

/**
 * Checks if the given symbol is a special char token
 * If yes, return the id
 * If not, return -1.
 **/
int checkSpecialToken(char * symbol);

/**
 * Deterministic-finite-automaton to be entered when an alpha character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Alpha(LexerState*);

/**
 * Deterministic-finite-automaton to be entered when a digit character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Digit(LexerState*);

/**
 * Deterministic-finite-automaton to be entered when a special character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Special(LexerState*);

/* ************************************************************************** */
/* Definitions ************************************************************** */
/* ************************************************************************** */

void initLexerState(LexerState* lexerState, char* sourceCode)
{
    lexerState->lineNum = 0;
    lexerState->charInd = 0;
    lexerState->sourceCode = sourceCode;
    lexerState->lexerError = NONE;

    initTokenList(&lexerState->tokenList);
}

int isCharacterValid(char c)
{
    return isalnum(c) || isspace(c) || isSpecialSymbol(c);
}

int isSpecialSymbol(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '(' || c == ')' || c == '=' || c == ',' ||
           c == '.' || c == '<' || c == '>' || c == ';' ||
           c == ':';
}

SymbolType getSymbolType(char c)
{
         if(isalpha(c))         return ALPHA;
    else if(isdigit(c))         return DIGIT;
    else if(isSpecialSymbol(c)) return SPECIAL;
    else                        return INVALID;
}

int checkReservedTokens(char* symbol)
{
    for(int i = firstReservedToken; i <= lastReservedToken; i++)
    {
        if( !strcmp(symbol, tokens[i]) )
        {
            // Symbol is the reserved token at index i.
            return i;
        }
    }

    // Symbol is not found among the reserved tokens
    return -1;
}

int checkSpecialToken(char * symbol)
{
    for (int i = 4; i < 20; i++)
    {
        if (i == 12) continue; //this is "odd", not a special sym
        if (!strcmp(symbol, tokens[i]))
        {
            return i;
        }
    }
    return -1;
}


/**
 * Deterministic-finite-automaton to be entered when an alpha character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Alpha(LexerState* lexerState)
{
    // There are two possible cases for symbols starting with alpha:
    // Case.1) A reserved token (a reserved word or 'odd')
    // Case.2) An ident

    // In both cases, symbol should not exceed 11 characters.
    // Read 11 or less alpha-numeric characters
    // If it exceeds 11 alnums, fill LexerState error and return
    // Otherwise, try to recognize if the symbol is reserved.
    //   If yes, tokenize by one of the reserved symbols
    //   If not, tokenize as ident.

    // Store the first character into the lexeme array
    char lexeme[12];
    //lexerState->charInd++;
    char c = lexerState->sourceCode[lexerState->charInd];

    // Length variable keeps track of 11 alphanumeric chars
    int lenCount = 0;

    // Loop as long as the char is alpha-numeric otherwise alnum
    while(isalnum(c))
    {
        lenCount++;
        // Check if the size exceeds 11. Throw error if it does
        if(lenCount > 11)
        {
            lexerState->lexerError = NAME_TOO_LONG;
            return;
        }
        lexeme[lenCount - 1] = c;
        lexerState->charInd++;
        c = lexerState->sourceCode[lexerState->charInd];
    }
    lexeme[lenCount] = '\0';

    // Check if the lexeme we have is part of the reserved tokens
    int checkVal = checkReservedTokens(lexeme);

    if(checkVal == -1)
    {
        // Not reserved so make it an id token
        Token newToken;
        newToken.id = identsym;
        strcpy(newToken.lexeme, lexeme);
        addToken(&lexerState->tokenList, newToken);
    }
    else {
        // Token is reserved
        Token newReservedToken;
        newReservedToken.id = checkVal;
        strcpy(newReservedToken.lexeme, lexeme);
        addToken(&lexerState->tokenList, newReservedToken);
    }

    return;
}


/**
 * Deterministic-finite-automaton to be entered when a digit character is seen.
 * Simulating a state machine, consumes the source code and changes the state
 * .. of the lexer (LexerState) as required. Possibly, adds new tokens to the
 * .. token list field of the LexerState.
 * If an error is encountered, sets the LexErr field of LexerState, sets the
 * .. line number field and returns.
 * */
void DFA_Digit(LexerState* lexerState)
{
    // There are three cases for symbols starting with number:
    // Case.1) It is a well-formed number
    // Case.2) It is an ill-formed number exceeding 5 digits - Lexer Error!
    // Case.3) It is an ill-formed variable name starting with digit - Lexer Error!

    // Tokenize as numbersym only if it is case 1. Otherwise, set the required
    // .. fields of lexerState to corresponding LexErr and return.

    char c = lexerState->sourceCode[lexerState->charInd];
    int length = 0;
    char lexeme[6];

    while (isdigit(c))
    {
        lexeme[length] = c;
        length++;

        if (length > 5)
        {
            //digits < 5
            lexerState->lexerError = NUM_TOO_LONG;
            return;
        }
        lexerState->charInd++;
        c = lexerState->sourceCode[lexerState->charInd];
    }
    
    lexeme[length] = '\0';
    
    if (isalpha(c))
    {
        lexerState->lexerError = NONLETTER_VAR_INITIAL;
        return;
    }

    Token newToken;
    newToken.id = numbersym;
    strcpy(newToken.lexeme, lexeme);
    addToken(&lexerState->tokenList, newToken);
}

void DFA_Special(LexerState* lexerState)
{
    // There are three cases for symbols starting with special:
    // Case.1: Beginning of a comment: "/*"
    // Case.2: Two character special symbol: "<>", "<=", ">=", ":="
    // Case.3: One character special symbol: "+", "-", "(", etc.

    // For case.1, you are recommended to consume all the characters regarding
    // .. the comment, and return. This way, lexicalAnalyzer() func can decide
    // .. what to do with the next character.

    // For case.2 and case.3, you could consume the characters, add the
    // .. corresponding token to the tokenlist of lexerState, and return.

    //comments
    if (lexerState->sourceCode[lexerState->charInd] == '/' && lexerState->sourceCode[lexerState->charInd + 1] == '*')
    {
        //we're in a comment
        lexerState->charInd += 2;
        char c = lexerState->sourceCode[lexerState->charInd];
        
        while (c != '\0')
        {
            if (c == '*' && lexerState->sourceCode[lexerState->charInd + 1] == '/')
            {
                lexerState->charInd += 2;
                return;
            }
            lexerState->charInd++;
            c = lexerState->sourceCode[lexerState->charInd];
        }
        return;
    }

    char c = lexerState->sourceCode[lexerState->charInd];
    int length = 0;
    char lexeme[2];

//     while (isSpecialSymbol(c))
//     {
//         lexeme[length] = c;
//         length++;
//         
//         if (length > 1)
//         {
//             //there are no specials symbols with lenth > 2
//             lexerState->charInd++;
//             break;
//         }
//         lexerState->charInd++;
//         c = lexerState->sourceCode[lexerState->charInd];
//     }
    int id;
    lexeme[2] = '\0';
    
    if (isSpecialSymbol(c)) {
        switch (c)
        {
            case '+':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = plussym;
                break;
            case '-':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = minussym;
                break;
            case '*':lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = multsym;
                break;
            case '/':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = slashsym;
                break;
            case '=':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = eqsym;
                break;
            case '<':
                lexerState->charInd++;
                lexeme[0] = c;
                if (lexerState->sourceCode[lexerState->charInd] == '>') {
                    lexerState->charInd++;
                    lexeme[1] = '>';
                    id = neqsym;
                    break;
                }
                else if (lexerState->sourceCode[lexerState->charInd] == '=') {
                    lexerState->charInd++;
                    lexeme[1] = '=';
                    id = leqsym;
                    break;
                }
                else {
                    lexeme[1] = '\0';
                    id = lessym;
                    break;
                }
                break;
            case '>':
                lexerState->charInd++;
                lexeme[0] = c;
                if (lexerState->sourceCode[lexerState->charInd] == '=') {
                    lexerState->charInd++;
                    lexeme[1] = '=';
                    id = geqsym;
                    break;
                }
                else {
                    lexeme[1] = '\0';
                    id = gtrsym;
                    break;
                }
                break;
            case '(':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = lparentsym;
                break;
            case ')':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = rparentsym;
                break;
            case ',':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = commasym;
                break;
            case ';':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = semicolonsym;
                break;
            case '.':
                lexerState->charInd++;
                lexeme[0] = c;
                lexeme[1] = '\0';
                id = periodsym;
                break;
            case ':':
                lexerState->charInd++;
                if (lexerState->sourceCode[lexerState->charInd] == '=') {
                    lexerState->charInd++;
                    lexeme[0] = ':';
                    lexeme[1] = '=';
                    id = becomessym;
                    break;
                }
                else
                    lexerState->lexerError = INV_SYM;
                break;
            default:
            {
                lexerState->lexerError = INV_SYM;
            }
        }
       
    }
    
    
    Token newToken;
    newToken.id = id;
    strcpy(newToken.lexeme, lexeme);
    addToken(&lexerState->tokenList, newToken);
}

LexerOut lexicalAnalyzer(char* sourceCode)
{
    if(!sourceCode)
    {
        fprintf(stderr, "ERROR: Null source code string passed to lexicalAnalyzer()\n");

        LexerOut lexerOut;
        lexerOut.lexerError = NO_SOURCE_CODE;
        lexerOut.errorLine = -1;

        return lexerOut;
    }

    // Create & init lexer state
    LexerState lexerState;
    initLexerState(&lexerState, sourceCode);

    // While not end of file, and, there is no lexer error
    // .. continue lexing
    while( lexerState.sourceCode[lexerState.charInd] != '\0' &&
        lexerState.lexerError == NONE )
    {
        char currentSymbol = lexerState.sourceCode[lexerState.charInd];

        // Skip spaces or new lines until an effective character is seen
        while(currentSymbol == ' ' || currentSymbol == '\n')
        {
            // Advance line number if required
            if(currentSymbol == '\n')
                lexerState.lineNum++;

            // Advance to the following character
            currentSymbol = lexerState.sourceCode[++lexerState.charInd];
        }

        // After recognizing spaces or new lines, make sure that the EOF was
        // .. not reached. If it was, break the loop.
        if(lexerState.sourceCode[lexerState.charInd] == '\0')
        {
            break;
        }

        // Take action depending on the current symbol's type
        switch(getSymbolType(currentSymbol))
        {
            case ALPHA:
                DFA_Alpha(&lexerState);
                break;
            case DIGIT:
                DFA_Digit(&lexerState);
                break;
            case SPECIAL:
                DFA_Special(&lexerState);
                break;
            case INVALID:
                lexerState.lexerError = INV_SYM;
                break;
        }
    }

    // Prepare LexerOut to be returned
    LexerOut lexerOut;

    if(lexerState.lexerError != NONE)
    {
        // Set LexErr
        lexerOut.lexerError = lexerState.lexerError;

        // Set the number of line the error encountered
        lexerOut.errorLine = lexerState.lineNum;

        lexerOut.tokenList = lexerState.tokenList;
    }
    else
    {
        // No error!
        lexerOut.lexerError = NONE;
        lexerOut.errorLine = -1;

        // Copy the token list

        // The scope of LexerState ends here. The ownership of the tokenlist
        // .. is being passed to LexerOut. Therefore, neither deletion of the
        // .. tokenlist nor deep copying of the tokenlist is required.
        lexerOut.tokenList = lexerState.tokenList;
    }

    return lexerOut;
}
