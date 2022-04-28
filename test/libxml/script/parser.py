import sys
import os
import json
import glob
import math

def parse_in(isn):

    mdic = {}
    tp = 0
    tf = 0

    for path in glob.glob("./output/" + str(isn)+"/*"):
        if not path.endswith(".symcov"):
            continue

        f = open(path, 'r', encoding='UTF-8')
        try:
            jf = json.load(f)
        except:
            print("ERROR: " + path)
        covered = jf['covered-points']
        if os.path.basename(path).startswith("p-"):
            tp = tp+1
        else:
            tf = tf+1

        for point in covered:
            if point in mdic:
                if not os.path.basename(path).startswith("p-"):
                    mdic[point] = [mdic[point][0], mdic[point][1]+1]
                else:
                    mdic[point] = [mdic[point][0]+1, mdic[point][1]]

            else:
                if not os.path.basename(path).startswith("p-"):
                    mdic[point] = [0, 1]
                else:
                    mdic[point] = [1, 0]

        f.close()

    return tp, tf, mdic

tp,tf,mdic = parse_in(sys.argv[1])


ndic_t = {}
ndic_j = {}
ndic_o = {}
ndic_s = {}

for k in mdic.keys():
    vlist = mdic[k]
    p = vlist[0]
    f = vlist[1]

    if tf != 0:
        _fail_t = f/tf
    else:
        _fail_t = 0

    _pass_t = p/tp

# tarantula
    sus_t = _fail_t / (_fail_t + _pass_t)
    con_t = max(_fail_t, _pass_t)

# jaccard
    sus_j = f / (p + tf)

# ochiai
    if (p+f)*tf != 0:
        sus_o = f / math.sqrt((p+f)*tf)
    else:
        sus_o = 0
# sbi
    sus_s = f / (p + f)

    ndic_t[k] = [sus_t, con_t]
    ndic_j[k] = sus_j
    ndic_o[k] = sus_j
    ndic_s[k] = sus_o


sorted_t = sorted(ndic_t.items(), key = lambda item: item[1], reverse = True)
sorted_j = sorted(ndic_j.items(), key = lambda item: item[1], reverse = True)
sorted_o = sorted(ndic_o.items(), key = lambda item: item[1], reverse = True)
sorted_s = sorted(ndic_s.items(), key = lambda item: item[1], reverse = True)

tp = open("./ret_"+sys.argv[1]+"_tarantula.txt", "w")
rank=1
for eg in sorted_t:
        #print(f"{rank} \t {eg[0]} \t {eg[1][0]} \t {eg[1][1]}")
        tp.write(f"{rank} \t {eg[0]} \t {eg[1][0]} \t {eg[1][1]}\n")
        rank=rank+1
tp.close()

tp = open("./ret_"+sys.argv[1]+"_jaccard.txt", "w")
rank=1
for eg in sorted_j:
        #print(f"{rank} \t {eg[0]} \t {eg[1]}")
        tp.write(f"{rank} \t {eg[0]} \t {eg[1]}\n")
        rank=rank+1
tp.close()

tp = open("./ret_"+sys.argv[1]+"_ochiai.txt", "w")
rank=1
for eg in sorted_o:
        #print(f"{rank} \t {eg[0]} \t {eg[1]}")
        tp.write(f"{rank} \t {eg[0]} \t {eg[1]}\n")
        rank=rank+1
tp.close()

tp = open("./ret_"+sys.argv[1]+"_sbi.txt", "w")
rank=1
for eg in sorted_s:
        #print(f"{rank} \t {eg[0]} \t {eg[1]}")
        tp.write(f"{rank} \t {eg[0]} \t {eg[1]}\n")
        rank=rank+1
tp.close()
