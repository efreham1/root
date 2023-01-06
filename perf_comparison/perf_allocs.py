import matplotlib.pyplot as plt
import csv

file = open("times.csv")

csvreader = csv.reader(file)

ns = []
times = []
times_malloc = []

for row in csvreader:
    ns.append(int(row[0]))
    times.append(float(row[1]))
    times_malloc.append(float(row[2]))

plt.plot(ns, times, 'b')
plt.plot(ns, times_malloc, 'r')
plt.savefig("allocs")