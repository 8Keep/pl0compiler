#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stdio.h>

#define MAX_LEXEME_LENGTH 11

/**
 * The struct to store token information
 * */
typedef struct {
    int id; // numerical representation of the token
    char lexeme[MAX_LEXEME_LENGTH + 1]; // null terminated string
} Token;

/**
 * The struct to store list of tokens and keep track
 * of number of tokens included in the list
 * */
typedef struct {
    Token* tokens;
    int numberOfTokens;
} TokenList;

/**
 * The struct that helps to iterate on a TokenList
 * */
typedef struct {
    TokenList* tokenList;
    int currentTokenInd;
} TokenListIterator;

/**
 * Initializes the given TokenList
 * */
void initTokenList(TokenList*);

/**
 * Adds the given Token to the given TokenList
 * */
void addToken(TokenList*, Token);

/**
 * Creates and returns a copy of the given TokenList.
 * TokenList dynamically allocates memory for its list.
 * Therefore, shallow copying with assignment operator does not
 * .. guarantee any lifetime of the included tokens list, which
 * .. may cause dangling pointers.
 * */
TokenList getCopy(TokenList);

/**
 * Writes the given TokenList to the given FILE
 * */
void printTokenList(TokenList, FILE*);

/**
 * Reads a list of tokens from given file.
 * The format of the list in the input file should be same as the printTokenList()
 * func prints.
 * */
TokenList readTokenList(FILE*);

/**
 * Makes the necessary deallocations on the TokenList
 * */
void deleteTokenList(TokenList*);

/**
 * Creates and returns a token list iterator
 * */
TokenListIterator getTokenListIterator(TokenList*);

/**
 * Returns the current token from TokenListIterator.
 * If the given iterator is invalid or all the tokens have already consumed,
 * .. returns nulsym.
 * */
Token getCurrentTokenFromIterator(TokenListIterator);

/**
 * Advances the position of next token of TokenListIterator by one.
 * */
void advanceTokenListIterator(TokenListIterator*);

#endif