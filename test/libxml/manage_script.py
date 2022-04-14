import os

fp = open("../xmllint_script.txt","r")

lines = fp.readlines()

print(len(lines))

n = 1
for l in lines:
    l = l[1:-1]
    
    comd =  "ASAN_OPTIONS=coverage=1 ./"+l + " 2> "+ str(n) +".temp > /dev/null"
    print(comd)
    os.system(comd)
    n = n+1
