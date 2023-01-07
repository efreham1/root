import matplotlib.pyplot as plt
import csv

file = open("perf_comparison/times_GC.csv")

csvreader = csv.reader(file)

ratio = []
times = []

for row in csvreader:
    ratio.append(float(row[0]))
    times.append(float(row[1]))

plt.plot(ratio, times, 'b', label="h_alloc_data of 24MB")
plt.xlabel("Ratio of actual data to allocated space [%]")
plt.ylabel("Time for one garbage collection [s]")
plt.legend()
plt.savefig("perf_comparison/GC")