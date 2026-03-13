import sys

out_file = open(sys.argv[1],"w+")
num_files = int(sys.argv[2])

header = ""

arg_idx = 3
for i in range(num_files):
    dataset_name = sys.argv[arg_idx]
    arg_idx+=1
    dataset_file = sys.argv[arg_idx]
    arg_idx+=1
    first = True
    for line in open(dataset_file,'r'):
        if first:
            first = False
            if header == '':
                header = line
                out_file.write(f"dataset,{line}")
            elif header != line:
                print("ERROR: input files have different headers")
                exit(1)
        else:
            out_file.write(f"{dataset_name},{line}")

