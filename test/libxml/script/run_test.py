import os
import sys

fp = open(sys.argv[1],"r")

lines = fp.readlines()

n = 1

def run_p(comd):
    print(comd)
    os.systen(comd)

for l in lines:
    l = l[1:-1]
    
    comd = "ASAN_OPTIONS=coverage=1 ./"+l + " 2> "+ str(n) +".err > /dev/null"
    run_p(comd)

    comd = ""

    n = n+1

    



