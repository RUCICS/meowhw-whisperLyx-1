#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>

// 计算两个数的最小公倍数
size_t lcm(size_t a, size_t b)
{
    size_t gcd_val = a;
    size_t temp = b;
    while (temp != 0)
    {
        size_t remainder = gcd_val % temp;
        gcd_val = temp;
        temp = remainder;
    }
    return (a * b) / gcd_val;
}

// 检查一个数是否是2的幂
int is_power_of_two(size_t n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

size_t io_blocksize(int fd)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
    {
        perror("sysconf");
        page_size = 4096;
    }

    struct stat st;
    if (fstat(fd, &st) == -1)
    {
        perror("fstat");
        return (size_t)page_size;
    }

    size_t fs_block_size = (size_t)st.st_blksize;

    // 处理虚假的块大小：如果不是2的幂或者小于512，使用默认值
    if (!is_power_of_two(fs_block_size) || fs_block_size < 512)
    {
        fs_block_size = 4096; // 使用常见的文件系统块大小
    }

    // 返回页大小和文件系统块大小的最小公倍数
    size_t buffer_size = lcm((size_t)page_size, fs_block_size);

    // 确保缓冲区大小不会过大（限制在1MB以内）
    if (buffer_size > 1024 * 1024)
    {
        buffer_size = (size_t)page_size;
    }

    return buffer_size;
}

char *align_alloc(size_t size)
{
    size_t page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
    {
        page_size = 4096;
    }

    size_t total_size = size + page_size;

    void *raw_ptr = malloc(total_size);
    if (raw_ptr == NULL)
    {
        return NULL;
    }

    uintptr_t raw_addr = (uintptr_t)raw_ptr;
    uintptr_t aligned_addr = (raw_addr + page_size - 1) & ~(page_size - 1);
    char *aligned_ptr = (char *)aligned_addr;

    *((void **)(aligned_ptr - sizeof(void *))) = raw_ptr;

    return aligned_ptr;
}

void align_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    void *raw_ptr = *((void **)((char *)ptr - sizeof(void *)));
    free(raw_ptr);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    size_t buf_size = io_blocksize(fd);
    char *buf = align_alloc(buf_size);
    if (buf == NULL)
    {
        perror("align_alloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    while ((bytes_read = read(fd, buf, buf_size)) > 0)
    {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read)
        {
            ssize_t result = write(STDOUT_FILENO, buf + bytes_written,
                                   bytes_read - bytes_written);
            if (result == -1)
            {
                perror("write");
                align_free(buf);
                close(fd);
                exit(EXIT_FAILURE);
            }
            bytes_written += result;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
        align_free(buf);
        close(fd);
        exit(EXIT_FAILURE);
    }

    align_free(buf);

    if (close(fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}