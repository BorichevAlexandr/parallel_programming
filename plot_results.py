import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

CSV_PATH = "results.csv"
OUTPUT_IMAGE = "performance_graph.png"
DPI = 150


if not os.path.exists(CSV_PATH):
    print(f"Файл '{CSV_PATH}' не найден!")
    sys.exit(1)

df = pd.read_csv(CSV_PATH)

if df.empty:
    print("CSV файл пустой!")
    sys.exit(1)

field_size = int(df['field_size'].iloc[0])

threads = df['threads'].values
times = df['avg_time_sec'].values


base_time = times[0]
speedup = base_time / times

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(13, 5.5))

ax1.plot(threads, times, marker='o', linewidth=2.5, markersize=9, color='#1f77b4')
ax1.set_xlabel("Количество потоков", fontsize=11)
ax1.set_ylabel("Среднее время, сек", fontsize=11)
ax1.set_title("Время выполнения одного шага", fontsize=12, fontweight='bold')
ax1.grid(True, linestyle='--', alpha=0.6)
ax1.set_xticks(threads)

for x, y in zip(threads, times):
    ax1.annotate(f"{y:.3f}", (x, y), textcoords="offset points",
                 xytext=(0, 10), ha='center', fontsize=9)

bars = ax2.bar(threads, speedup, color='#2ca02c', alpha=0.85, edgecolor='black', width=0.6)
ax2.axhline(1.0, color='gray', linestyle='--', linewidth=1.2)
ax2.set_xlabel("Количество потоков", fontsize=11)
ax2.set_ylabel("Ускорение (Speedup)", fontsize=11)
ax2.set_title("Ускорение относительно 1 потока", fontsize=12, fontweight='bold')
ax2.grid(axis='y', linestyle='--', alpha=0.6)
ax2.set_xticks(threads)

for bar, sp in zip(bars, speedup):
    height = bar.get_height()
    ax2.annotate(f"{sp:.2f}x",
                 xy=(bar.get_x() + bar.get_width() / 2, height),
                 xytext=(0, 5),
                 textcoords="offset points",
                 ha='center', va='bottom', fontsize=10, fontweight='bold')


fig.suptitle(f"Conway's Game of Life — OpenMP\nРазмер поля: {field_size} × {field_size}",
             fontsize=14, fontweight='bold', y=1.02)

plt.tight_layout()
plt.savefig(OUTPUT_IMAGE, dpi=DPI, bbox_inches='tight')

os.startfile(OUTPUT_IMAGE)   