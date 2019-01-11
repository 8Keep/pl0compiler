tests="tests.txt"
cg="../code_generator.out"
vm="../vm/vm.out"
timeout=1s

i=0

# check if cg.out, vm.out and tests_grader.txt exists
if [[ -e $cg && -e $vm && -e $tests ]] ; then
    echo "$cg, $vm and $tests are found. Starting tests.."
else
    echo "$cg, $vm or $tests could not be found! Aborting.."
    exit
fi

# pl0_code, vm_inp and gt_vm_out will be given.
# cg_out and vm_out is expected to be outputted by the pipeline and will be
#   the files that are going to be graded.
# cg_in    : Input to code generator, which is a list of lexemes.
# vm_inp   : Input to pl0_code, inputted to vm while running the generated code.
#            Might be /dev/null for some cases where cg_out is expected to be an
#            error.
# cg_out   : Output of code generator. Might include pm0 code or error.
# vm_out   : Output of vm after running the pm0 code given in cg_out.
# gt_vm_out: Expected vm_out. Might be /dev/null for some cases where cg_out is
#            expected to be an error.
while read is_err cg_in cg_out others; do
    echo "TEST[$i]"
    # resolve others depending on whether it is an error case or not
    if [ "$is_err" = "not_error" ]; then
      # resolve other filenames
      others_array=($others)
      vm_inp=${others_array[0]}
      vm_out=${others_array[1]}
      gt_vm_out=${others_array[2]}
    elif [ "$is_err" = "error" ]; then
      gt_cg_out=$others
    else
      echo "ERROR WHILE RUNNING TESTER SCRIPT: error or not_error?"
      exit 0
    fi

    # create directories if needed
    out_dir=$(dirname "$cg_out")
    mkdir -p "$out_dir"
    out_dir=$(dirname "$vm_out")
    mkdir -p "$out_dir"
    
    # run the code generator
    (timeout $timeout "$cg" "$cg_in" "$cg_out") > /dev/null 2>&1
    

    # if the error case is expected, then, do not run vm
    if [ "$is_err" = "not_error" ]; then
      # code should have been produced. therefore, run the vm.
      (timeout $timeout "$vm" "$cg_out" "/dev/null" "$vm_inp" "$vm_out") > /dev/null 2>&1
    fi

    
    let i=$i+1

done < "$tests"

echo "# of test cases completed: $i"