#include "source_code.h"
#include <stdio.h>
#include <stdlib.h>

char* readSourceCode(FILE * inp)
{
    if(!inp)
        return NULL;

    // How much chars should be added when the allocated
    // .. space gets full
    const int charCountAdvanceAmount = 10;

    // Initially, no space is allocated
    int allocatedCharCount = 0;
    char* sourceCode = NULL;

    // Index to be filled by the newly read character
    int nextCharInd = 0;

    char currentChar;
    while( (currentChar = fgetc(inp)) != EOF )
    {
        while(allocatedCharCount <= nextCharInd + 1)
        {
            allocatedCharCount += charCountAdvanceAmount;
            sourceCode = (char*)realloc(sourceCode, allocatedCharCount * sizeof(char));
        }

        sourceCode[nextCharInd++] = currentChar;
    }

    // Put terminator character at the end - if applicable
    if(sourceCode) sourceCode[nextCharInd++] = '\0';

    return sourceCode;
}

void deleteSourceCode(char* sourceCode)
{
    if(sourceCode)
        free(sourceCode);
}

void printSourceCode(char* sourceCode)
{
    if(!sourceCode)
        return;

    int i = 0;
    while(sourceCode[i++] != '\0')
    {
        printf("%c", sourceCode[i]);
    }
}