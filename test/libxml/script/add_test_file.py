# 243_issue/
import os
import sys

test_file = sys.argv[1] # poc
issue_str = sys.argv[2] # 243

path_list = ["/test/", "/test/namespaces/", "/test/xmlid/", "/test/errors/", "/test/errors10/", "/test/XInclude/docs/", "/test/scripts/", "/test/SVG/", "/test/VCM/", "/test/VC/", "/test/valid/", "/test/relaxng/", "/test/schematron/", "/test/pattern/"]

for p in path_list:
    comd = "cp ../" + test_file + " ." + p + "issue_"+issue_str+".xml"
    print(comd)
    os.system(comd)
