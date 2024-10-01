#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "encDec.h"

//(working)Read file and turn them into frames of length  32
char* read_file(char* filename, off_t* offset,int buffer_size) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    if (lseek(fd, *offset, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        return NULL;
    }

    char* buffer = malloc(buffer_size + 1); // +1 for null terminator
    ssize_t bytes_read = read(fd, buffer, buffer_size);
    if (bytes_read == -1) {
        perror("read");
        free(buffer);
        close(fd);
        return NULL;
    }

    if (bytes_read == 0) {
        free(buffer);
        close(fd);
        return NULL;
    }

    buffer[bytes_read] = '\0'; // add null terminator
    *offset += bytes_read;
    close(fd);
    return buffer;
}