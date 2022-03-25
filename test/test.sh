echo "#Test Jsondump with Corpus"

target_program="./jsondump"
multiple_main="../bin/main"
PI_dir="../test/corpus"
PO_dir="../bin/result"

cp -r ../result ../bin/

# EXECUTE
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 ../bin/main $target_program $PI_dir $PO_dir -bcov | tee result.txt

rm temp.bcov
