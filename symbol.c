#include "symbol.h"
#include <stdlib.h>
#include <string.h>

void initSymbolTable(SymbolTable* symbolTable)
{
    symbolTable->symbols = NULL;
    symbolTable->numberOfSymbols = 0;
}

void deleteSymbolTable(SymbolTable* symbolTable)
{
    if(!symbolTable) return;

    if(symbolTable->symbols)
        free(symbolTable->symbols);

    symbolTable->symbols = NULL;
    symbolTable->numberOfSymbols = 0;
}

Symbol* addSymbol(SymbolTable* symbolTable, Symbol symbol)
{
    if(!symbolTable) return NULL;

    symbolTable->numberOfSymbols++;

    symbolTable->symbols = (Symbol*)realloc(symbolTable->symbols, (symbolTable->numberOfSymbols) * sizeof(Symbol));

    symbolTable->symbols[symbolTable->numberOfSymbols - 1] = symbol;

    return &symbolTable->symbols[symbolTable->numberOfSymbols - 1];
}

void printSymbolTable(SymbolTable* symbolTable, FILE* out)
{
    if(!symbolTable || !out) return;

    fprintf(out, "Symbol Table\n============\n");

    for(int i = 0; i < symbolTable->numberOfSymbols; i++)
    {
        fprintf(out, "#%d\n", i);

        Symbol* symbol = &(symbolTable->symbols[i]);

        switch(symbol->type)
        {
            case VAR:
                fprintf(out, 
                    "   Type: VAR\n"
                    "   Name: %s\n"
                    "  Level: %d\n",
                    symbol->name, symbol->level);
                    break;

            case CONST:
                fprintf(out, 
                    "   Type: CONST\n"
                    "   Name: %s\n"
                    "  Value: %d\n"
                    "  Level: %d\n",
                    symbol->name, symbol->value, symbol->level);
                    break;

            case PROC:
                fprintf(out, 
                    "   Type: PROC\n"
                    "   Name: %s\n"
                    "  Level: %d\n",
                    symbol->name, symbol->level);
                    break;
        }

        // Print backtrace of scope
        fprintf(out, "  Scope: ");
        Symbol* scope = symbol->scope;
        while(scope != NULL)
        {
            fprintf(out, "%s -> ", scope->name);
            scope = scope->scope;
        }
        fprintf(out, "GLOBAL\n\n");
    }
}

Symbol* findSymbol(SymbolTable* symbolTable, Symbol* scope, const char* symbolName)
{
    if(!symbolTable || !symbolName) return NULL;

    // Search from the most inner scope to global scope
    while(1)
    {
        // Search the current scope
        for(int i = 0; i < symbolTable->numberOfSymbols; i++)
        {
            if( symbolTable->symbols[i].scope == scope && !strcmp(symbolTable->symbols[i].name, symbolName) )
            {
                return &symbolTable->symbols[i];
            }
        }

        if(!scope)
        {
            /**
             * All symbols from the most inner scope to global scope are searched.
             * However, the symbol is not found. 
             * */
            return NULL;
        } else
        {
            // Change the scope to the outer scope
            scope = scope->scope;
        }
    }

}
