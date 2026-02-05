import matplotlib.pyplot as plt
import numpy as np
import sys
import os

data_folder = sys.argv[1]
for data_file in os.listdir(data_folder):
    if not data_file.endswith('.csv'):
        continue
    data = np.genfromtxt(f"{data_folder}/{data_file}", delimiter=',',names=True,dtype=None)

    names = list(data.dtype.names)[1:]
    penguin_means = {
        'Mean': (18.35, 18.43, 14.98,5),
        '95th percentile': (38.79, 48.83, 47.50,5)
    }

    x = np.arange(len(names))  # the label locations
    width = 0.25  # the width of the bars
    multiplier = 0

    fig, ax = plt.subplots(layout='constrained')

    for attribute, measurement in penguin_means.items():
        offset = width * multiplier
        rects = ax.bar(x + offset, measurement, width, label=attribute)
        ax.bar_label(rects, padding=3)
        multiplier += 1

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('Length (mm)')
    ax.set_title('Penguin attributes by species')
    ax.set_xticks(x + width, names)
    ax.legend(loc='upper left', ncols=3)
    ax.set_ylim(0, 250)

    plt.savefig(f"{data_folder}/{"".join(data_file.split('.')[:-1])}.png")