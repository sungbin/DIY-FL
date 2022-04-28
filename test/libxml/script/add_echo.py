import sys
import re

pat = r"\$\(top_builddir\)\/(xmllint(\$\(EXEEXT\))?(\s--\w+){0,4}(\s(\$\$i\s\$\$j|\$\$i|\$\$xml\s\<\s\$\$i|\$\$pat\s\$\(srcdir\)\S+|dba100000.xml|\$\(srcdir\)\S+|\$\$pat))(\s\$\$i)?)"
p = re.compile(pat)

def build_echo(comd):
    #comd: xmllint$(EXEEXT) --schematron $$i $$j
    #return: echo "$(CHECKER) [$(top_builddir)/xmllint $$i"] >> ../txt.txt ; \

    return ("echo \"$(CHECKER) ") + comd + ("\" >> ../xmllint_script.txt ; \\")

make_fp = open(sys.argv[1], "r")

lines = make_fp.readlines()
new_lines = []

for i in range(len(lines)):
    line = lines[i]
    line = line[:-1]
    new_lines.append(line)

    m = p.search(line)
    if m:

        fs = m.group(1)
        if fs[-1] == ';':
            fs = fs[:-1]

        #print(build_echo(fs))
        while ";" not in line:
            i = i+1
            line = lines[i]
            new_lines.append(line[:-1])
        
        new_lines.append(build_echo(fs))

fp = open("./Makefile", "w")

for l in new_lines:
    fp.write(l + "\n")

fp.close()
