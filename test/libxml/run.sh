# ..
rm xmllint_script.txt
cd ./243_libxml2
rm Makefile
cd ..

python script.py ./243_libxml2/_Makefile
mv ./Makefile ./243_libxml2
cd ./243_libxml2
../run_tests.sh
