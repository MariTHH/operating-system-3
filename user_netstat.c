#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PROCFS_PATH "/proc/netstat_custom"

void print_usage(const char *progname) {
    printf("Usage: %s [--filter <filter>]\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (argc == 3 && strcmp(argv[1], "--filter") == 0) {
            int fd = open(PROCFS_PATH, O_WRONLY);
            if (fd < 0) {
                perror("Failed to open proc file for writing");
                return EXIT_FAILURE;
            }

            if (write(fd, argv[2], strlen(argv[2])) < 0) {
                perror("Failed to write filter to proc file");
                close(fd);
                return EXIT_FAILURE;
            }
            close(fd);
        } else {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    FILE *file = fopen(PROCFS_PATH, "r");
    if (!file) {
        perror("Failed to open proc file");
        return EXIT_FAILURE;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }

    fclose(file);
    return EXIT_SUCCESS;
}
