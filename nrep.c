#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

enum {
    MaxName = 512,
    Bufnum = 8 * 1024,
};

void repname(char *path, int n, char *name) {
    char *pointer;

    pointer = strrchr(path, '/');
    if (pointer == NULL) {
        pointer = path;
    } else {
        pointer++;
        if (*pointer == '\0') {
            errx(EXIT_FAILURE, "bad path: %s", path);
        }
    }
    snprintf(name, MaxName, "%s.%d", pointer, n);
}

void donrep(char *path, int num) {
    int fd, i, j;
    char name[MaxName];
    char buf[Bufnum];
    int nr;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        err(EXIT_FAILURE, "can't open file %s", path);
    }

    for (i = 1; i <= num; i++) {
        repname(path, i, name);
        int fdout = open(name, O_WRONLY | O_TRUNC | O_CREAT, 0664);
        if (fdout < 0) {
            err(EXIT_FAILURE, "can't open file %s", name);
        }

        for (j = 0; j < i; j++) {
            lseek(fd, 0, SEEK_SET);
            while ((nr = read(fd, buf, Bufnum)) > 0) {
                if (write(fdout, buf, nr) != nr) {
                    err(EXIT_FAILURE, "write failed");
                }
            }
            if (nr < 0) {
                err(EXIT_FAILURE, "read failed");
            }
        }
        close(fdout);
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: nrep FILE num\n");
        exit(EXIT_FAILURE);
    }

    int num = atoi(argv[2]);
    if (num <= 0) {
        fprintf(stderr, "num must be > 0\n");
        exit(EXIT_FAILURE);
    }

    donrep(argv[1], num);

    return 0;
}