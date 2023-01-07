import matplotlib.pyplot as plt
import csv

file = open("perf_comparison/times_GC_vs_free.csv")

csvreader = csv.reader(file)

depth = []
times1 = []
times2 = []

for row in csvreader:
    depth.append(float(row[0]))
    times1.append(float(row[1]))
    times2.append(float(row[2]))

plt.plot(depth, times1, 'b',
         label="Deleting a linked structure with h_gc()")

plt.plot(depth, times2, 'r',
         label="Deleting a linked structure with free()")

plt.xlabel("Depth of the linked structure [n]")
plt.ylabel("Time for deletion [s]")
plt.legend()
plt.savefig("perf_comparison/GC_vs_free")
