tests="tests.txt"
cg="../code_generator.out"
vm="../vm/vm.out"
EMPH='\033[1;31m'
GREEN_EMPH='\033[1;32m'
DEEMPH='\033[0m'
timeout=1s

i=0
passed=0
failed=0

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
    echo -e "${GREEN_EMPH}TEST[$i]${DEEMPH}"
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
      echo "ERROR WHILE RUNNING GRADER SCRIPT: error or not_error in $tests?"
      exit 0
    fi

    # create directories if needed
    out_dir=$(dirname "$cg_out")
    mkdir -p "$out_dir"
    out_dir=$(dirname "$vm_out")
    mkdir -p "$out_dir"
    
    # run the code generator
    (timeout $timeout "$cg" "$cg_in" "$cg_out") > /dev/null 2>&1

    # if the error case is expected, then, do not run vm but just check the err
    if [ "$is_err" = "error" ]; then
      # check if the correct error code is produced
      _diff=$( { diff -B -w $cg_out $gt_cg_out; } 2>&1 )

    elif [ "$is_err" = "not_error" ]; then
      # code should have been produced. therefore, run the vm.
      (timeout $timeout "$vm" "$cg_out" "/dev/null" "$vm_inp" "$vm_out") > /dev/null 2>&1

      # check if the correct vm_out is produced
      _diff=$( { diff -B -w $vm_out $gt_vm_out; } 2>&1 )
    fi
    
    # up to now, _diff should have been already set up
    
    if [[ $_diff ]] ; then
        # sad.. difference found
        echo "TEST $i FAILED"
        let failed=$failed+1

        if [ "$is_err" = "error" ]; then
          echo "There is difference between $cg_out and $gt_cg_out:"
          echo "=================================================================="
          echo $_diff
          echo "=================================================================="
          echo "Your code generator was expected to output an error code for the given input."
          echo -e "${EMPH}Test this yourself by running the following${DEEMPH}: "
          echo "  (cd test/; ./$cg $cg_in $cg_out)"
          echo "The output is in \"test/$cg_out\". It was expected to match \"test/$gt_cg_out\"."
          echo ""
        elif [ "$is_err" = "not_error" ]; then
          echo "There is difference between $vm_out and $gt_vm_out:"
          echo "=================================================================="
          echo $_diff
          echo "=================================================================="
          echo "Your code generator was expected to output a PM0 code that would produce a certain"
          echo "output when it is run on the virtual machine."
          echo -e "${EMPH}Test this yourself by running the following${DEEMPH}: "
          echo "  (cd test/; ./$cg $cg_in $cg_out)"
          echo "  (cd test/; ./$vm $cg_out /dev/null $vm_inp $vm_out) "
          echo "The output is in \"test/$vm_out\". It was expected to match \"test/$gt_vm_out\"."
          echo ""
        fi
    else
        # yay! test passed
        echo "TEST $i PASSED"
        let passed=$passed+1
    fi
    let i=$i+1

done < "$tests"

echo "# of tests       : $i"
echo "# of tests passed: $passed"
echo "# of tests failed: $failed"
