import re
import struct
import functools
import glob

path_set = set()

def parse(fname):
    f = open(fname,'rb')
    f.read(4) # Total Branch
    data = []

    while (byte := f.read(4)):
        if len(byte) !=  4:
            break
        branch = struct.unpack('I', byte)
        byte = f.read(4)
        if len(byte)  != 4:
            break
        des_len = struct.unpack('I', byte)
        byte = f.read(des_len[0])
        if len(byte) != des_len[0]:
            break
        des = byte.decode()
        if des in path_set:
                continue
        else:
            #print(branch[0], des)
            data.append( [branch[0], des] )
            path_set.add(des)

    return data


path = "./result/*"
file_list = glob.glob(path)
bcov_list = [file for file in file_list if file.endswith(".bcov")]


all_list = []
for bcov in bcov_list:
    data = parse(bcov)
    all_list.extend(data)

all_list.sort()

for one in all_list:
    print(one[0], one[1])
