import matplotlib.pyplot as plt
import pandas as pd

# 读取结果
df = pd.read_csv('buffer_test_results.txt')

# 创建图表
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# 读取速度图表
ax1.plot(df['倍数'], df['读取速度(MB/s)'], 'b-o', label='读取速度')
ax1.set_xlabel('缓冲区大小倍数（相对于页面大小）')
ax1.set_ylabel('读取速度 (MB/s)')
ax1.set_title('读取速度 vs 缓冲区大小')
ax1.grid(True)
ax1.legend()

# 写入速度图表
ax2.plot(df['倍数'], df['写入速度(MB/s)'], 'r-o', label='写入速度')
ax2.set_xlabel('缓冲区大小倍数（相对于页面大小）')
ax2.set_ylabel('写入速度 (MB/s)')
ax2.set_title('写入速度 vs 缓冲区大小')
ax2.grid(True)
ax2.legend()

plt.tight_layout()
plt.savefig('buffer_performance.png', dpi=300, bbox_inches='tight')
plt.show()

# 找到最优倍数
max_read_idx = df['读取速度(MB/s)'].idxmax()
max_write_idx = df['写入速度(MB/s)'].idxmax()

print(f"\n最优读取性能: {df.loc[max_read_idx, '倍数']}x 页面大小, 速度: {df.loc[max_read_idx, '读取速度(MB/s)']} MB/s")
print(f"最优写入性能: {df.loc[max_write_idx, '倍数']}x 页面大小, 速度: {df.loc[max_write_idx, '写入速度(MB/s)']} MB/s")

# 计算性能不再显著提升的点
read_speeds = df['读取速度(MB/s)'].values
write_speeds = df['写入速度(MB/s)'].values

# 寻找性能提升小于5%的点
optimal_read_mult = 1
optimal_write_mult = 1

for i in range(1, len(read_speeds)):
    if read_speeds[i] > 0 and read_speeds[i-1] > 0:
        improvement = (read_speeds[i] - read_speeds[i-1]) / read_speeds[i-1]
        if improvement < 0.05:  # 小于5%提升
            optimal_read_mult = df.loc[i-1, '倍数']
            break

for i in range(1, len(write_speeds)):
    if write_speeds[i] > 0 and write_speeds[i-1] > 0:
        improvement = (write_speeds[i] - write_speeds[i-1]) / write_speeds[i-1]
        if improvement < 0.05:  # 小于5%提升
            optimal_write_mult = df.loc[i-1, '倍数']
            break

print(f"\n推荐的缓冲区倍数（读取）: {optimal_read_mult}x")
print(f"推荐的缓冲区倍数（写入）: {optimal_write_mult}x")
print(f"综合推荐: {max(optimal_read_mult, optimal_write_mult)}x")
