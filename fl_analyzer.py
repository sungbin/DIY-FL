import re
import struct
import functools
import glob

def parse_sus(fname):
    f = open(fname,'rb')
    data = f.read()
    f.close()
    (sus) = struct.unpack('f',data)
    return sus

def parse_suscon(fname):
    f = open(fname,'rb')
    data = f.read()
    f.close()
    (sus, con) = struct.unpack('ff',data)
    return sus, con

def bring_suscon_dic(suscon_list):

    dic = {}
    for f in suscon_list:
        (sus, con) = parse_suscon(f)
        dic[f] = [sus, con]

    return dic

def bring_sus_dic(sus_list):

    dic = {}
    for f in sus_list:
        (sus) = parse_sus(f)
        dic[f] = sus

    return dic

def rank_dic_list(dic,sist):

    r = 1
    for i, tup in enumerate(sist):

        k = tup[0]
        val = tup[1]

        dic[k] = r
        r = r+1
        if i < 1:
            continue

        if sist[i-1][1] == val:
            r = r-1
            dic[k]= r

def analysis_sus_print(dic):
    rank = 1
    pre_rank = 1
    for i, tup in enumerate(dic):
        if i == 0:
            print(tup[0], tup[1], rank)
            continue
    
        elif dic[i-1][1] != tup[1]:
            rank = rank + 1
            print(tup[0], tup[1], pre_rank)
        else:
            rank = rank + 1
            print(tup[0], tup[1], rank)
            pre_rank = rank

p = re.compile(r'(\d+)\.(suscon|ssus|osus|jsus)', re.IGNORECASE)
def compare(x, y):
    return x[0] - y[0]

def analysis_sus(dic):
    new_dic = {}
    rank = 1
    pre_rank = 1
    for i, tup in enumerate(dic):
        m = p.search(tup[0]);
        n = int(m.group(1))
        if i == 0:
            new_dic[n] = rank;
            continue
    
        elif dic[i-1][1] != tup[1]:
            rank = rank + 1
            new_dic[n] = pre_rank;
        else:
            rank = rank + 1
            new_dic[n] = rank;
            pre_rank = rank

    return new_dic


path = "./result/*"
file_list = glob.glob(path)

suscon_list = [file for file in file_list if file.endswith(".suscon")]
ssus_list = [file for file in file_list if file.endswith(".ssus")]
jsus_list = [file for file in file_list if file.endswith(".jsus")]
osus_list = [file for file in file_list if file.endswith(".osus")]

suscon_dic = bring_suscon_dic(suscon_list);
ssus_dic = bring_sus_dic(ssus_list);
jsus_dic = bring_sus_dic(jsus_list);
osus_dic = bring_sus_dic(osus_list);

dict1 = sorted(suscon_dic.items(), key = lambda item: item[1], reverse = True)
dict2 = sorted(ssus_dic.items(), key = lambda item: item[1], reverse = True)
dict3 = sorted(jsus_dic.items(), key = lambda item: item[1], reverse = True)
dict4 = sorted(osus_dic.items(), key = lambda item: item[1], reverse = True)

#analysis_sus_print(dict1)
#analysis_sus_print(dict2)
#analysis_sus_print(dict3)
#analysis_sus_print(dict4)

rank_dic1 = analysis_sus(dict1)
rank_dic2 = analysis_sus(dict2)
rank_dic3 = analysis_sus(dict3)
rank_dic4 = analysis_sus(dict4)

path_list = sorted(rank_dic4.items(), key=functools.cmp_to_key(compare), reverse = False)

for k, val in path_list:
    print(k, rank_dic1[k], rank_dic2[k], rank_dic3[k], rank_dic4[k])
