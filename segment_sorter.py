import sys
from collections import defaultdict

f = open(sys.argv[1],'r')
columns = defaultdict(list)

for line in f:
    split = line.split(",")
    start = int(split[0].split(":")[1])
    count = int(split[1].split(":")[1])
    column = int(split[2].split(":")[1])
    columns[column].append((start,count))

sorted_keys = []

for key in columns:
    columns[key].sort()
    sorted_keys.append(key)

sorted_keys.sort()

for key in sorted_keys:
    for tpl in columns[key]:
        print(key,tpl[1])

