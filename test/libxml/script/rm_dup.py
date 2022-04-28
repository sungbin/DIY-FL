import sys

script_file = sys.argv[1] 


rp = open(script_file, "r")
lines = rp.readlines()


lines = set(lines)
rp.close()

wp = open(script_file, "w")
for l in lines:
    wp.write(l)
    print(l)
wp.close()
'''
'''
