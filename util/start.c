#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#define PREFIX "dnao-"
#define GAMEBIN "dnethack"

char const * const path_fmt[] = {"/%s/%s.0",
                                 "/%s/save/%s",
                                 "/%s/save/%s.gz",
                                 "/%s/save/%s.bz2",
                                 "/%s/save/%s.e",
                                 NULL};

int main(int argc, char **argv) {
    if (argc < 2) return 111;

    char path[PATH_MAX + 1];
    path[PATH_MAX] = '\0';

    DIR *root = opendir("/");
    struct dirent *ent;
    while ((ent = readdir(root))) {
        if (ent->d_type != DT_DIR) {
            continue;
        }

        if (strncmp(ent->d_name, PREFIX, strlen(PREFIX))) {
            continue;
        }

        char const * const *fmt;
        for (fmt = path_fmt; *fmt; fmt++) {
            snprintf(path, PATH_MAX, *fmt, ent->d_name, argv[1]);
            if (!access(path, F_OK)) {
                snprintf(path, PATH_MAX, "/%s/" GAMEBIN, ent->d_name);
                goto found;
            }
        }
    }

    snprintf(path, PATH_MAX, "/%s/" GAMEBIN, PREFIX "cur");
found:
    argv[1] = path;
    execv(path, argv + 1);
    return 127;
}
