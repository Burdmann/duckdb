import sys
import random

f = open(sys.argv[1],"w+")
n = int(sys.argv[2])
max_val = int(sys.argv[3])

f.write("longs\n")
for i in range(n):
    f.write(f"{random.randint(0,max_val)}\n")