rm -r jsmn
git clone https://github.com/zserge/jsmn.git
cd jsmn
git checkout 6784c826d9674915a4d89649c6288e6aecb4110d

clang -g -c -O3 -g -fsanitize=address -fsanitize-coverage=trace-pc-guard example/jsondump.c -o example/jsondump.o
clang -g -c -O3 -g -fsanitize=address -fsanitize-coverage=trace-pc-guard jsmn.c -o jsmn.o
clang -g -fsanitize=address -fsanitize-coverage=trace-pc-guard -o jsondump example/jsondump.o jsmn.o ../../bin/trace-pc.o

rm ../jsondump
cp ./jsondump ..
mv jsondump ..
cd ..
