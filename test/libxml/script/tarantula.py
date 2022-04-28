import sys
import os
import json
import glob

import parser

tp, tf, mdic = parser.parse_in()

ndic = {}

for k in mdic.keys():
    vlist = mdic[k]
    p = vlist[0]
    f = vlist[1]

    if tf != 0:
        _fail = f/tf
    else:
        _fail = 0

    _pass = p/tp

    sus = _fail / (_fail + _pass)
    con = max(_fail, _pass)

    ndic[k] = [sus, con]

sorted_d = sorted(ndic.items(), key = lambda item: item[1], reverse = True)

rank=1
for eg in sorted_d:
    print(f"{rank} \t {eg[0]} \t {eg[1][0]} \t {eg[1][1]}")
    rank=rank+1

#print("tf",tf,"tp",tp)
