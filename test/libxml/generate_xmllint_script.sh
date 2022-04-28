# ..
rm xmllint_script.txt
rm Makefile

python script/add_echo.py ./$1_libxml2/_Makefile # need _Makefile
rm ./$1_libxml2/Makefile
mv ./Makefile ./$1_libxml2

cd ./$1_libxml2

python ../script/add_test_file.py $1.crash $1
../script/run_tests.sh

cd ..

python ./script/rm_dup.py ./xmllint_script.txt
echo "total test case: $(cat xmllint_script.txt | wc -l)"
