#!/usr/bin/python3
import re
import os
import sys
#f=open("raw.out")
if len(sys.argv)<3:
    print("Usage: ./hexaddr_to_linenum.py <output file> <binary file>")
    sys.exit()
f=open(sys.argv[1],'r')
#f=open("converted.out","w")
lines=f.readlines()
lines=''.join(lines)
#print(lines)
last_pos=0
for l,r in [(m.start(0), m.end(0)) for m in re.finditer('0[xX][0-9a-fA-F]+', lines)]:
    #print('matches')
    #print(l,r)
    #print(lines[l:r])
    linenum=os.popen('addr2line {} -e {}'.format(lines[l:r],sys.argv[2])).read()
    print(lines[last_pos:l],end='')
    print(linenum[:-1],end='')
    last_pos=r

print(lines[last_pos:-1],end='')
