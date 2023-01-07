import matplotlib.pyplot as plt
import csv

file = open("perf_comparison/times_linked_lists.csv")

csvreader = csv.reader(file)

M = []
times1 = []
times2 = []
times3 = []

for row in csvreader:
    M.append(float(row[0]))
    times1.append(float(row[1]))
    times2.append(float(row[2]))
    times3.append(float(row[3]))

plt.plot(M, times1, 'b',
         label="Linked lists with own heap and no gc")

plt.plot(M, times2, 'r',
         label="Linked lists with own heap and with gc")

plt.plot(M, times3, 'g',
         label="Linked_lists with malloc")

plt.xlabel("M [#]")
plt.ylabel("Runtime [s]")
plt.legend()
plt.savefig("perf_comparison/linked_lists")
