#!/usr/bin/env python3
import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# ================= STYLE =================
BAR_W = 0.18
COLORS = ["black", "dimgray", "darkgray", "silver", "gray"]
EDGE = "black"
EDGE_W = 0.8
YGRID = dict(axis="y", linestyle="--", linewidth=0.5, alpha=0.6)

# Font sizes
FONTS = {
    "bottom_xtick": 12,
    "ytick": 12,
    "ylabel": 14,
    "xlabel": 16,
    "legend": 12
}

X_LABEL_TEXT = "Queries"
X_LABEL_PAD = 24


def plot_from_csv(csv_path, out_path):
    df = pd.read_csv(csv_path)

    # Ignore first row
    df = df.reset_index(drop=True)

    # Method columns (everything after column index 2)
    method_cols = df.columns[3:]
    data = df[method_cols].astype(float)

    n_queries = len(df)
    n_methods = len(method_cols)

    x = np.arange(n_queries)
    w = BAR_W

    fig_w = max(12, 0.9 * n_queries + 4)
    plt.figure(figsize=(fig_w, 4))
    ax = plt.gca()

    offsets = np.linspace(-(n_methods - 1) / 2,
                          (n_methods - 1) / 2,
                          n_methods) * w

    for i, col in enumerate(method_cols):
        ax.bar(x + offsets[i],
               data[col],
               width=w,
               label=col,
               color=COLORS[i % len(COLORS)],
               edgecolor=EDGE,
               linewidth=EDGE_W)
    
    ax.margins(x=0.01)

    # X-axis labels: Q1, Q2, ...
    query_labels = [f"Q{i+1}" for i in range(n_queries)]
    ax.set_xticks(x)
    ax.set_xticklabels(query_labels,
                       fontsize=FONTS["bottom_xtick"])
    ax.tick_params(axis='y',
                   labelsize=FONTS["ytick"])

    ax.set_ylabel("Query time (seconds)",
                  fontsize=FONTS["ylabel"])
    ax.set_xlabel(X_LABEL_TEXT,
                  fontsize=FONTS["xlabel"],
                  labelpad=X_LABEL_PAD)

    ax.legend(loc="upper center",
              ncol=min(4, n_methods),
              fontsize=FONTS["legend"])
    ax.grid(**YGRID)

    ymax = np.nanmax(data.values)
    ax.set_ylim(0, ymax * 1.15 if ymax > 0 else 1)

    plt.tight_layout()
    plt.savefig(out_path,
                format="pdf",
                bbox_inches="tight",
                pad_inches=0.1)
    plt.close()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--out", default="figure.pdf")
    args = ap.parse_args()

    plot_from_csv(args.csv, args.out)


if __name__ == "__main__":
    main()