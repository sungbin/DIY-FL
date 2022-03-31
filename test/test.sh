echo "#Test Jsondump with Corpus"

multiple_main="../bin/main"
PI_dir="../test/corpus"
PO_dir="../bin/result"
target_program="./jsondump"

# EXECUTE
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 ../bin/main $PI_dir $PO_dir $target_program -bcov |& tee result.txt
