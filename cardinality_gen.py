import sys

in_file = open(sys.argv[1],"r")
out_file = open(sys.argv[2],"w+")
granularity = int(sys.argv[3])

idx = 0
for row in in_file:
    if idx == 0:
        out_file.write(row)
    else:
        time,data = map(int,row.split(","))
        time = time // granularity
        out_file.write(f"{time},{data}\n")
    if (idx % 10000 == 0):
        print(idx)
    idx += 1