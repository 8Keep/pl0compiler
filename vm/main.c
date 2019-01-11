#include <stdio.h>
#include <string.h>
#include "vm.h"

int main(int argc, char **argv)
{
    FILE *inp, *outp, *vm_inp, *vm_outp;

    if(argc == 3)
    {
        inp     = fopen(argv[1], "r");
        outp    = fopen(argv[2], "w");

        vm_inp  = stdin;
        vm_outp = stdout;

        simulateVM(inp, outp, vm_inp, vm_outp);

        fclose(inp);
        fclose(outp);
    }
    else if(argc == 5)
    {
        inp     = fopen(argv[1], "r");
        outp    = fopen(argv[2], "w");

        // vm_inp
        if( strcmp(argv[3], "-") ) vm_inp = fopen(argv[3], "r");
        else                       vm_inp = stdin;

        // vm_outp
        if( strcmp(argv[3], "-") ) vm_outp = fopen(argv[4], "w");
        else                       vm_outp = stdout;

        simulateVM(inp, outp, vm_inp, vm_outp);

        fclose(inp);
        fclose(outp);

        // vm_inp : close the file stream if it is not stdin
        if( strcmp(argv[3], "-") ) fclose(vm_inp);

        // vm_outp: close the file stream if it is not stdout
        if( strcmp(argv[4], "-") ) fclose(vm_outp);
    }
    else
    {
        fprintf(stderr, "Usage: vm.out (ins_inp_file) (simul_outp_file) [vm_inp_file=stdin] [vm_outp_file=stdout]\n");

        fprintf(stderr, "\n\tins_inp_file  The path to the file containing the list of instructions to"
                        "\n\t              be loaded to code memory of the virtual machine.\n");

        fprintf(stderr, "\n\tsimul_outp_file  The path to the file to write the simulation output, which"
                        "\n\t                 contains both code memory and execution history.\n");
        fprintf(stderr, "\n\tvm_inp_file  The path to the file that is going to be attached as the input"
                        "\n\t             stream to the virtual machine. Useful to feed input for SIO"
                        "\n\t             instructions. Use dash ('-') to assign to stdin.\n");
        fprintf(stderr, "\n\tvm_outp_file The path to the file that is going to be attached as the output"
                        "\n\t             stream to the virtual machine. Useful to save the output printed"
                        "\n\t             by SIO instructions. Use dash ('-') to assign to stdout.\n");
    }

    return 0;
}
