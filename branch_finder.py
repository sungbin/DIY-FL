import re
import struct
import functools
import glob

def parse(fname):
    data = {}
    f = open(fname,'rb')
    f.read(4) # Total Branch

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
        data[branch[0]] = des

    return data


path = "./result/*"
file_list = glob.glob(path)
bcov_list = [file for file in file_list if file.endswith(".bcov")]

bcov_des_dic = {}
for bcov in bcov_list:
    data = parse(bcov)
    bcov_des_dic.update(data)


bn = input("")
print(bcov_des_dic[int(bn)])




