#ifndef __LEXICAL_ANALYZER_H__
#define __LEXICAL_ANALYZER_H__

#include "token.h"
#include <stdio.h>

/**
 * Enumaration for possible lexer errors.
 * */
typedef enum {
    NONE = 0,
    NONLETTER_VAR_INITIAL,
    NAME_TOO_LONG,
    NUM_TOO_LONG,
    INV_SYM,
    NO_SOURCE_CODE
} LexErr;


/**
 * LexerOut struct: the return value of lexicalAnalyzer() func
 * */
typedef struct {
    /**
     * The list of tokens to be filled by lexicalAnalyzer(). The essential
     * .. field that should be filled after successful execution of lexical
     * .. analysis, ie., when encountered no errors.
     * */
    TokenList tokenList;

    /**
     * Should be filled when an error is encountered while lexical analysis.
     * Otherwise, should be kept as NONE to signal success.
     * Be careful that the errorLine field should also be filled with the number
     * .. of the line the error was encountered when LexErr is different than
     * .. NONE.
     * */
    LexErr lexerError;

    /**
     * Should be filled when LexErr is encountered. Indicates the number of line
     * .. that the LexErr is encountered.
     * */
    int errorLine;

} LexerOut;

/**
 * Deallocates the memory dynamically allocated for the members of LexerOut.
 * */
void deleteLexerOut(LexerOut*);

/**
 * Does lexical analysis on the given source code.
 * If the analysis is successful, i.e. no errors in the given source code,
 * .. returns a LexerOut with lexerError=LexErr::NONE, and a TokenList filled
 * .. with tokens.
 * If the analysis is NOT successful, i.e. errors are found in the given source
 * .. code, returns a LexerOut with lexerError and errorLine fields properly set.
 * */
LexerOut lexicalAnalyzer(char* sourceCode);

#endif