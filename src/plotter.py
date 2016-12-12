#!/usr/bin/env python2

import sys
from ast import literal_eval

import matplotlib.pyplot as plt

XS = literal_eval(sys.argv[1])
YS1 = literal_eval(sys.argv[2])
YS2 = literal_eval(sys.argv[3])
X_NAME = sys.argv[4]
Y1_NAME = sys.argv[5]
Y2_NAME = sys.argv[6]
OUTPUT_FILENAME = sys.argv[7]

xs1, ys1 = zip(*[(x, y) for x, y in zip(XS, YS1) if y != 'nan'])
xs2, ys2 = zip(*[(x, y) for x, y in zip(XS, YS2) if y != 'nan'])

fig, ax1 = plt.subplots()
ax1.plot(xs1, ys1, "b-")
ax1.set_xlabel(X_NAME)
ax1.set_ylabel(Y1_NAME, color="b")
for tl in ax1.get_yticklabels():
    tl.set_color('b')

ax2 = ax1.twinx()
ax2.plot(xs2, ys2, "r-")
ax2.set_ylabel(Y2_NAME, color="r")
for tl in ax2.get_yticklabels():
    tl.set_color('r')

plt.savefig(OUTPUT_FILENAME)
