import matplotlib.pyplot as plt
import csv

file = open("perf_comparison/times.csv")

csvreader = csv.reader(file)

ns = []
times = []
times_malloc = []

for row in csvreader:
    ns.append(int(row[0]))
    times.append(float(row[1]))
    times_malloc.append(float(row[2]))

plt.plot(ns, times, 'b', label="h_alloc_data of 2048MB")
plt.plot(ns, times_malloc, 'r', label="malloc of 2048MB")
plt.xlabel("Number of allocations [#]")
plt.ylabel("Time [s]")
plt.legend()
plt.savefig("perf_comparison/allocs")