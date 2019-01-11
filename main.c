#include <stdio.h>
#include "token.h"
#include "code_generator.h"

int main(int argc, char **argv)
{
    FILE *inp, *outp;

    /**********************************/
    /* Parse Command Line Arguments */
    /**********************************/
    if(argc != 3)
    {
        fprintf(stderr, "Usage: ./code_generator.out (pl0_lexer_out) (cg_output_file)\n");

        fprintf(stderr, "\n       pl0_lexer_out: The path to the file containing the lexer out for the programming language PL/0.\n");

        fprintf(stderr, "\n       cg_output_file: The path to the file to write the code generator output, which could contain either PM/0 assembly code or code generator error message.\n");
        return -1;
    }

    // open the input file for reading
    if( !(inp = fopen(argv[1], "r")) )
    {
        fprintf(stderr, "Could not open \"%s\"\n", argv[1]);
        return -1;
    }

    // open the output file for writing
    if( !(outp = fopen(argv[2], "w")) )
    {
        fprintf(stderr, "Could not open \"%s\"\n", argv[2]);

        // Before terminating, close the input file
        fclose(inp);

        return -1;
    }              

    /**********************************/
    /**** Call to code generator   ****/
    /**********************************/
    // Read the token list
    TokenList tokenList = readTokenList(inp);
    
    // Run code generator
    int err = codeGenerator(tokenList, outp);

    // Print error - if there exists any
    if(err) printCGErr(err, outp);

    // Delete token list created by readTokenList()
    deleteTokenList(&tokenList);

    /**********************************/
    /* Closing input and output files */
    /**********************************/

    // close the input and the output file stream
    if(inp) fclose(inp);
    if(outp) fclose(outp);

    return 0;
}
