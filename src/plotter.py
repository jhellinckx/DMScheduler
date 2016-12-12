#!/usr/bin/env python2

import sys
from ast import literal_eval

import matplotlib.pyplot as plt

XS = literal_eval(sys.argv[1])
YS1 = literal_eval(sys.argv[2])
YS2 = literal_eval(sys.argv[3])
X_NAME = sys.argv[4]
Y_NAME = sys.argv[5]
Y1_LABEL = sys.argv[6]
Y2_LABEL = sys.argv[7]
OUTPUT_FILENAME = sys.argv[8]

LEGEND_LOCATION = "best"

xs1, ys1 = zip(*[(x, y) for x, y in zip(XS, YS1) if y != 'nan'])
xs2, ys2 = zip(*[(x, y) for x, y in zip(XS, YS2) if y != 'nan'])

plt.plot(xs1, ys1, "b-", label=Y1_LABEL)
plt.plot(xs2, ys2, "b-", label=Y2_LABEL)
plt.xlabel(X_NAME)
plt.ylabel(Y_NAME)
plt.legend(loc=LEGEND_LOCATION)

plt.savefig(OUTPUT_FILENAME)
