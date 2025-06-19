import matplotlib.pyplot as plt
import numpy as np

# --- REPLACE WITH YOUR ACTUAL MEASURED MEAN TIMES IN MILLISECONDS ---
programs = ['System cat', 'mycat1', 'mycat2', 'mycat3', 'mycat4', 'mycat5', 'mycat6']
# Times in milliseconds for better scale on y-axis if mycat1 is included
times_ms = [
    456,      # System cat
    1500000,  # mycat1 (char-by-char)
    779,      # mycat2 (page-size buffer)
    775,      # mycat3 (aligned page-size buffer)
    848,      # mycat4 (fs_block_size aware, aligned)
    436,      # mycat5 (256KB buffer, aligned) - EXAMPLE, use your value
    436       # mycat6 (256KB buffer, aligned, fadvise) - EXAMPLE, use your value
]
# --- END OF DATA TO REPLACE ---

# If mycat1's time is excessively large, it will dwarf other bars.
# Consider plotting two charts: one with all, one without mycat1 for better detail.

# Plot 1: All programs
plt.figure(figsize=(12, 7))
bars = plt.bar(programs, times_ms, color=['skyblue', 'salmon', 'lightgreen', 'gold', 'lightcoral', 'mediumpurple', 'lightseagreen'])
plt.ylabel('Mean Execution Time (ms)')
plt.title('Performance Comparison of cat Implementations (All)')
plt.xticks(rotation=45, ha="right")
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Add text labels on bars
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2.0, yval + 0.05 * max(times_ms), f'{yval:.0f}', ha='center', va='bottom', fontsize=8)

plt.tight_layout()
plt.savefig('mycat_performance_all.png')
print("Plot 'mycat_performance_all.png' saved.")
# plt.show()

# Plot 2: Excluding mycat1 for better detail on optimized versions
programs_optimized = programs[0:1] + programs[2:] # System cat + mycat2 onwards
times_ms_optimized = times_ms[0:1] + times_ms[2:]

plt.figure(figsize=(10, 6))
bars_optimized = plt.bar(programs_optimized, times_ms_optimized, color=['skyblue', 'lightgreen', 'gold', 'lightcoral', 'mediumpurple', 'lightseagreen'])
plt.ylabel('Mean Execution Time (ms)')
plt.title('Performance Comparison of cat Implementations (Optimized Versions)')
plt.xticks(rotation=45, ha="right")
plt.grid(axis='y', linestyle='--', alpha=0.7)

# Add text labels on bars for the optimized plot
for bar in bars_optimized:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2.0, yval + 0.02 * max(times_ms_optimized), f'{yval:.0f}', ha='center', va='bottom')

plt.tight_layout()
plt.savefig('mycat_performance_optimized.png')
print("Plot 'mycat_performance_optimized.png' saved.")
# plt.show()