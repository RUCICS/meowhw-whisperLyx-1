#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

size_t io_blocksize(void)
{
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1)
    {
        perror("sysconf");
        // 如果获取页面大小失败，使用常见的4K作为备选
        return 4096;
    }
    return (size_t)page_size;
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
    char *buf = malloc(buf_size);
    if (buf == NULL)
    {
        perror("malloc");
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
                free(buf);
                close(fd);
                exit(EXIT_FAILURE);
            }
            bytes_written += result;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
        free(buf);
        close(fd);
        exit(EXIT_FAILURE);
    }

    free(buf);

    if (close(fd) == -1)
    {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}