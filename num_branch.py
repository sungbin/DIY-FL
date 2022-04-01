import re
import struct
import functools
import glob

def parse(fname):

    cnt = 0

    data = []
    f = open(fname,'rb')
    f.read(4) # Total Branch
    
    cnt = cnt+4

    s = len("./bin/result/")
    all_branch_fname = fname[:s] + fname[s+2:-5] + ".bcovnum"
    nf = open(all_branch_fname, 'w')

    while (byte := f.read(4)):
        one = []

        if len(byte) !=  4:
            break
        cnt = cnt+4
        branch = struct.unpack('I', byte)

        byte = f.read(4)
        if len(byte)  != 4:
            break
        cnt = cnt+4
        des_len = struct.unpack('I', byte)

        byte = f.read(des_len[0])
        if len(byte) != des_len[0]:
            break
        cnt = cnt+des_len[0]

        print(hex(branch[0]), des_len[0], byte.decode())

        one.append(branch[0])
        one.append(des_len[0])

        nf.write(str(branch[0]))
        nf.write(" ")

        data.append(one)

    f.close()
    nf.close()

    
    return data,cnt


path = "./bin/result/*"
file_list = glob.glob(path)
bcov_list = [file for file in file_list if file.endswith(".bcov")]

bcov_des_dic = []
for bcov in bcov_list:
    if "./bin/result/"+"16" in bcov:
        continue
    data = parse(bcov)
    print(data)
    #bcov_des_dic.extend(data)


 #print(bcov_des_dic)
'''
 '''
