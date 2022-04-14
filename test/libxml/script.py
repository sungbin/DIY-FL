import sys
import re

pat = r"\$\(top_builddir\)\/(xmllint(\$\(EXEEXT\))?(\s--\w+){0,4}(\s(\$\$i\s\$\$j|\$\$i|\$\$xml\s\<\s\$\$i|\$\$pat\s\$\(srcdir\)\S+|dba100000.xml|\$\(srcdir\)\S+|\$\$pat))(\s\$\$i)?)"
p = re.compile(pat)

for line in sys.stdin:
    #print(line[:-1])
    line = line[:-1]

    m = p.search(line)
    if m:

        fs = m.group(1)
        if fs[-1] == ';':
            fs = fs[:-1]

        print(fs)
