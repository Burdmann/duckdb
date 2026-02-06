import matplotlib.pyplot as plt
import numpy as np
import sys
import os

data_folder = sys.argv[1]
for data_file in os.listdir(data_folder):
    if not data_file.endswith('.csv'):
        continue
    data = np.atleast_1d(np.genfromtxt(f"{data_folder}/{data_file}", delimiter=',',names=True,dtype=None,deletechars="",replace_space=" "))
    rows = data.size
    cols = len(data[0])
    means = [np.mean([data[j][i] for j in range(rows)]) for i in range(3,cols)]
    p95 = [np.percentile([data[j][i] for j in range(rows)],95) for i in range(3,cols)]
    title = data[0][0]
    unit = data[0][1]

    names = list(data.dtype.names)[3:]
    penguin_means = {
        'Mean': means,
        '95th percentile': p95
    }

    x = np.arange(len(names))  # the label locations
    width = 0.25  # the width of the bars
    multiplier = 0

    fig, ax = plt.subplots(layout='constrained')

    for attribute, measurement in penguin_means.items():
        offset = width * multiplier
        rects = ax.bar(x + offset, measurement, width, label=attribute)
        ax.bar_label(rects, padding=3,fmt="%.3g")
        multiplier += 1

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel(f'{title}{f" ({unit})" if unit else ""}')
    ax.set_title(title)
    ax.set_xticks(x + width, names)
    ax.legend(loc='upper left', ncols=3)
    ax.set_ymargin(0.2)

    plt.savefig(f"{data_folder}/{"".join(data_file.split('.')[:-1])}.png")