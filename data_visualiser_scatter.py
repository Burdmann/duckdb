import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from collections import OrderedDict
from sigfig import round


def get_cmap(n, name='hsv'):
    '''Returns a function that maps each index in 0, 1, ..., n-1 to a distinct 
    RGB color; the keyword argument name must be a standard mpl colormap name.'''
    return plt.get_cmap(name, n)

input_file = sys.argv[1]
title = sys.argv[2]
output_folder = sys.argv[3]

data = np.atleast_1d(np.genfromtxt(input_file, delimiter=',',names=True,dtype=None,deletechars="",replace_space=" "))

groups = list({row[0] for row in data})
groups.sort()
idxs = {}
for i,group in enumerate(groups):
    idxs[group] = i
cmap = get_cmap(len(groups)+1)
for group in groups:
    c=cmap(idxs[group])
    group_data = [row for row in data if row[0] == group]
    group_data.sort(key=lambda row: row[3])
    plt.scatter([row[3] for row in group_data],[row[2] for row in group_data],c=[c],label=group)
    plt.plot([row[3] for row in group_data],[row[2] for row in group_data],c=c)
plt.legend()
plt.title(title)
plt.xlabel("Metadata size (GB)")
plt.ylabel("Rows scanned (millions)")
plt.xlim(left=0)
plt.ylim(bottom=-10,top=None)
plt.margins(2,2)
plt.tight_layout()

plt.savefig(f"{output_folder}/scatter.png")