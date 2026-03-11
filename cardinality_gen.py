import sys
import random

in_file = open(sys.argv[1],"r")
out_file = open(sys.argv[2],"w+")
granularity = int(sys.argv[3])
# granularity = int(sys.argv[1])

random.seed(42)

ids = list(range(granularity))

# for i in range(1000):
#     print(random.randint(0,granularity-1))

old_partition = -1
selection = []

idx = 0
for row in in_file:
    partition = (idx-1) // 8192
    if partition != old_partition:
        old_partition = partition
        selection = random.sample(ids,int(0.5*granularity))
    if idx == 0:
        out_file.write(row)
    else:
        time,_ = map(int,row.split(","))
        out_file.write(f"{time},{random.choice(selection)}\n")
    
    if (idx % 10000 == 0):
        print(idx)
    idx += 1