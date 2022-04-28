cd $1_libxml2 
rm *.sancov
rm *.temp
python ../script/run_test.py ../xmllint_script.txt > ./executed.txt
