import matplotlib.pyplot as plt
import numpy as np
import sys
import os
from collections import OrderedDict
from sigfig import round

data_folder = sys.argv[1]
time_file = f"{data_folder}/query_time.csv"
scan_count_file = f"{data_folder}/pruning.csv"
files = [(time_file,'Time'),(scan_count_file,"Rows scanned")]
data_means = {}
data_percentiles = {}
axs = {}
units = {}
for data_file,name in files:
    data = np.atleast_1d(np.genfromtxt(data_file, delimiter=',',names=True,dtype=None,deletechars="",replace_space=" "))
    rows = data.size
    cols = len(data[0])
    means = [np.mean([data[j][i] for j in range(rows)]) for i in range(3,cols)]
    p95 = [np.max([data[j][i] for j in range(rows)]) for i in range(3,cols)]
    p5 = [np.min([data[j][i] for j in range(rows)]) for i in range(3,cols)]
    unit = data[0][1]
    units[name] = unit

    names = list(data.dtype.names)[3:]
    data_means[name] = means
    if rows >= 20:
        data_percentiles[name] = (p5,p95)


x = np.arange(len(names))  # the label locations
width = 0.25  # the width of the bars
gap = 0.1
multiplier = 0

fig, ax1 = plt.subplots(layout='constrained')
ax2 = ax1.twinx()
axs["Time"] = ax1
axs["Rows scanned"] = ax2
colours = {"Time": 'tab:blue',"Rows scanned":'tab:red'}

for attribute, measurement in data_means.items():
    ax = axs[attribute]
    offset = (width+gap) * multiplier
    colour = colours[attribute]
    rects = ax.bar(x + offset, measurement, width, label=attribute,color=colour)
    ax.set_axisbelow(False)
    if attribute in data_percentiles:
        ybots = (np.array(measurement)-np.array(data_percentiles[attribute][0])).clip(0)
        ytops = (np.array(data_percentiles[attribute][1])-np.array(measurement)).clip(0)
        # print(ybots)
        # print(ytops)
        # print(measurement)
        # print("p5",np.array(data_percentiles[attribute][0]))
        # print("p95",np.array(data_percentiles[attribute][1]))
        # print("mean",np.array(measurement))
        percentiles = ax.errorbar(x + offset, measurement,(ybots,ytops), label="Min & max values",color='black',fmt='none',capsize=15,elinewidth=0)
    ax.bar_label(rects, padding=3, fmt=lambda x: round(x,sigfigs=3))
    multiplier += 1

    # Add some text for labels, title and custom x-axis tick labels, etc.
    unit = units[attribute]
    ax.set_ylabel(f'{attribute}{f" ({unit})" if unit else ""}',color=colour)
    ax.tick_params(axis='y', labelcolor=colour)
    ax.set_title("Pruning & Time")
    ax.set_xticks(x + width, names)
ax1.set_ymargin(0.2)
ax2.set_ymargin(0.2)
handles = []
labels = []
h, l = ax1.get_legend_handles_labels()
handles += h
labels += l
h, l = ax2.get_legend_handles_labels()
handles += h
labels += l

by_label = OrderedDict(zip(labels, handles))
ax1.legend(by_label.values(), by_label.keys(),loc='upper left', ncols=3)

plt.savefig(f"{data_folder}/processed.png")