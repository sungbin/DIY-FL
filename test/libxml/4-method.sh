cd output/$1
pwd
rm *.symcov
python sancov_generator.py $1
cd ../..


rm ret_$1*
rm $1_ret.tar
python ./script/parser.py $1

tar -cvf $1_ret.tar ret_$1*
