#!/usr/bin/env python2

import sys
from ast import literal_eval

import matplotlib.pyplot as plt

XS = literal_eval(sys.argv[1])
YS = literal_eval(sys.argv[2])
X_NAME = sys.argv[3]
Y_NAME = sys.argv[4]
OUTPUT_FILENAME = sys.argv[5]

xs, ys = zip(*[(x, y) for x, y in zip(XS, YS) if y != 'nan'])

plt.plot(xs, ys)
plt.xlabel(X_NAME)
plt.ylabel(Y_NAME)
plt.savefig(OUTPUT_FILENAME)
