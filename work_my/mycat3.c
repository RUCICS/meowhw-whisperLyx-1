#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

size_t io_blocksize(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
    {
        perror("sysconf");
        return 4096;
    }
    return (size_t)page_size;
}

char *align_alloc(size_t size)
{
    size_t page_size = io_blocksize();
    // 分配额外的空间来确保能够找到页对齐的地址
    size_t total_size = size + page_size;

    void *raw_ptr = malloc(total_size);
    if (raw_ptr == NULL)
    {
        return NULL;
    }

    // 计算页对齐的地址
    uintptr_t raw_addr = (uintptr_t)raw_ptr;
    uintptr_t aligned_addr = (raw_addr + page_size - 1) & ~(page_size - 1);
    char *aligned_ptr = (char *)aligned_addr;

    // 在对齐指针前8字节存储原始指针，用于后续释放
    *((void **)(aligned_ptr - sizeof(void *))) = raw_ptr;

    return aligned_ptr;
}

void align_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    // 从对齐指针前8字节获取原始指针
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

    size_t buf_size = io_blocksize();
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