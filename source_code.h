#ifndef __SOURCE_CODE_H__
#define __SOURCE_CODE_H__

#include <stdio.h>

/**
 * Reads the source code from file until EOF to a null-terminated string.
 * */
char* readSourceCode(FILE*);

/**
 * Prints the source code - simply prints a string.
 * */
void printSourceCode(char*);

/**
 * Deallocates the memory dynamically allocated for source code string
 * .. - simply calls free on the pointer.
 * */
void deleteSourceCode(char*);

#endif