#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

char const * const path_fmt[] = {"%s/%s.0",
                                 "%s/save/%s",
                                 "%s/save/%s.gz",
                                 "%s/save/%s.bz2",
                                 "%s/save/%s.e",
                                 "%s/save/%s.e.gz",
                                 "%s/save/%s.e.bz2",
                                 NULL};

int main(int argc, char **argv) {
    /* start prefix- 1337name prefix-cur ./game [options...] */
    if (argc < 5) return 111;

    int prefixlen = strlen(argv[1]);

    char path[PATH_MAX + 1];
    path[PATH_MAX] = '\0';

    DIR *root = opendir(".");
    if (!root) return 112;

    struct dirent *ent;
    while ((ent = readdir(root))) {
        if (ent->d_type != DT_DIR) {
            continue;
        }

        if (strncmp(ent->d_name, argv[1], prefixlen)) {
            continue;
        }

        char const * const *fmt;
        for (fmt = path_fmt; *fmt; fmt++) {
            snprintf(path, PATH_MAX, *fmt, ent->d_name, argv[2]);
            if (!access(path, F_OK)) {
                chdir(ent->d_name);
                goto found;
            }
        }
    }

    chdir(argv[3]);
found:
    execv(argv[4], argv + 4);
    return 127;
}
