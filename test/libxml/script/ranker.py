import os
import re
import glob
import json



def grap(put):
    for path in glob.glob("./output/*"):
        if not path.endswith(".symcov"):
            continue

        f = open(path, 'r', encoding='UTF-8')
        jf = json.load(f)
        table = jf['point-symbol-info']
        f.close()
        for t in table.keys():
            ps = table[t]
            for p in ps.keys():
                k = table[t][p]
                for j in k.keys():
                    if put == j:
                        return str(p)+str(k[j])


file_list = os.listdir('.')

pt = r'(\d)\s+(\S+)\s+(\S+)'
p = re.compile(pt)


for f in file_list:
    if not f.startswith("ret") or ".txt" not in f:
        continue

    print(f)
    
    np = open("./_" + f, "w")
    fp = open(f, "r")
    lines = fp.readlines()
    
    for l in lines:
        if len(l) < 6:
            continue

        md = p.search(l)
        idx = md.group(2)

        #print(idx)
        #print(l[:-1] + " \t " + grap(idx))
        np.write(l[:-1] + " \t " + grap(idx) + "\n")



np.close()
fp.close()
