import re
import struct
import functools
import glob

def parse(fname):
    f = open(fname,'rb')

    fn = 0
    pn = 0
    while (byte := f.read(1)):
        val = struct.unpack('B', byte)
        if val[0] == 1:
            fn = fn+1
        else:
            pn = pn+1

    bn = int(fname[len("./bin/result/*")-1:fname.find(".branch")])
    return [bn, pn, fn]

path = "./bin/result/*"
file_list = glob.glob(path)
branch_list = [file for file in file_list if file.endswith(".branch")]

_l = []
for bcov in branch_list:
    data = parse(bcov)
    _l.append(data)


_l.sort()

for l in _l:
    print(l[0], l[1], l[2])


