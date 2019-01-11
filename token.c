#include "token.h"
#include <stdio.h>
#include <stdlib.h>

void initTokenList(TokenList* tokenList)
{
    tokenList->tokens = NULL;
    tokenList->numberOfTokens = 0;
}

void addToken(TokenList* tokenList, Token token)
{
    // Increase number of tokens
    tokenList->numberOfTokens++;

    // Allocate space for new token
    tokenList->tokens = (Token*)realloc(tokenList->tokens, (tokenList->numberOfTokens) * sizeof(Token));

    // Add token to the end of the list
    tokenList->tokens[tokenList->numberOfTokens - 1] = token;
}

TokenList getCopy(TokenList src)
{
    TokenList copy;
    
    copy.numberOfTokens = src.numberOfTokens;

    if(src.tokens)
    {
        copy.tokens = (Token*)malloc(src.numberOfTokens * sizeof(Token));

        for(int i = 0; i < src.numberOfTokens; i++)
            copy.tokens[i] = src.tokens[i];
    }

    return copy;
}

void printTokenList(TokenList tokenList, FILE* out)
{
    if(out == NULL || tokenList.tokens == NULL)
        return;

    fprintf(out, "%10s   %12s\n", "Token Type", "Lexeme");

    for(int i = 0; i < tokenList.numberOfTokens; i++)
    {
        fprintf(out, "%10d   %12s\n", tokenList.tokens[i].id, tokenList.tokens[i].lexeme);
    }
}

TokenList readTokenList(FILE* in)
{
    TokenList tokenList;

    tokenList.tokens = NULL;
    tokenList.numberOfTokens = 0;

    if(!in) return tokenList;

    // Skip header, which is 26 characters
    fseek(in, 26, SEEK_CUR);

    Token token;

    while( fscanf(in, "%10d   %12s\n", &token.id, token.lexeme) == 2 )
    {
        addToken(&tokenList, token);
    }

    return tokenList;
}

void deleteTokenList(TokenList* tokenList)
{
    if(!tokenList) return;
    
    if(tokenList->tokens)
        free(tokenList->tokens);

    tokenList->tokens = NULL;
}


TokenListIterator getTokenListIterator(TokenList* tokenList)
{
    TokenListIterator it;

    it.currentTokenInd = 0;

    if(tokenList) it.tokenList = tokenList;
    else          it.tokenList = NULL;

    return it;
}

Token getCurrentTokenFromIterator(TokenListIterator it)
{
    if(!it.tokenList || !it.tokenList->tokens || it.currentTokenInd >= it.tokenList->numberOfTokens)
    {
        Token nulsymToken = { .id=0, .lexeme="" };
        return nulsymToken;
    }

    return it.tokenList->tokens[it.currentTokenInd];
}

void advanceTokenListIterator(TokenListIterator* it)
{
    if(it) it->currentTokenInd++;
}