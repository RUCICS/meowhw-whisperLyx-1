CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99

# 目标程序
TARGETS = mycat1 mycat2 mycat3 mycat4 mycat5 mycat6

# 默认目标
all: $(TARGETS)

# 原始的单字节读取cat（已提供）
mycat1: mycat1.c
	$(CC) $(CFLAGS) -o $@ $<

# 带缓冲区的cat
mycat2: mycat2.c
	$(CC) $(CFLAGS) -o $@ $<

# 缓冲区对齐的cat
mycat3: mycat3.c
	$(CC) $(CFLAGS) -o $@ $<

# 考虑文件系统块大小的cat
mycat4: mycat4.c
	$(CC) $(CFLAGS) -o $@ $<

# 考虑系统调用开销的cat
mycat5: mycat5.c
	$(CC) $(CFLAGS) -o $@ $<

# 使用fadvise优化的cat
mycat6: mycat6.c
	$(CC) $(CFLAGS) -o $@ $<

# 性能测试
test: all
	@echo "创建测试文件..."
	@dd if=/dev/zero of=test_file bs=1M count=10 2>/dev/null
	@echo "测试各个版本的性能："
	@echo "系统cat:"
	@hyperfine --warmup 3 'cat test_file > /dev/null'
	@echo "mycat1 (单字节):"
	@hyperfine --warmup 3 './mycat1 test_file > /dev/null'
	@echo "mycat2 (缓冲区):"
	@hyperfine --warmup 3 './mycat2 test_file > /dev/null'
	@echo "mycat3 (对齐缓冲区):"
	@hyperfine --warmup 3 './mycat3 test_file > /dev/null'
	@echo "mycat4 (文件系统块大小):"
	@hyperfine --warmup 3 './mycat4 test_file > /dev/null'
	@echo "mycat5 (系统调用优化):"
	@hyperfine --warmup 3 './mycat5 test_file > /dev/null'
	@echo "mycat6 (fadvise优化):"
	@hyperfine --warmup 3 './mycat6 test_file > /dev/null'
	@rm -f test_file

# 清理
clean:
	rm -f $(TARGETS) test_file test_file_* temp_write_test buffer_test_results.txt buffer_performance.png

# 缓冲区大小测试
buffer-test:
	@chmod +x buffer_size_test.sh
	@./buffer_size_test.sh

.PHONY: all test clean buffer-test