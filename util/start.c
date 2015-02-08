#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define PREFIX "dnao-"
#define GAMEBIN "dnethack"

int main(int argc, char **argv) {
    DIR *root;
    struct dirent* ent;
    char *path;

    if (argc < 2) return 111;

    root = opendir("/");
    if (!root) return 112;

    while ((ent = readdir(root))) {
        if (ent->d_type != DT_DIR)
            continue;

        if (strncmp(ent->d_name, PREFIX, strlen(PREFIX)))
            continue;

        asprintf(&path, "/%s/%s.0", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            printf("game in progress: %s", path);
            return 113;
        }
        free(path);

        asprintf(&path, "/%s/save/%s.e", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            printf("save error: %s", path);
            return 114;
        }
        free(path);

        asprintf(&path, "/%s/save/%s", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/" GAMEBIN, ent->d_name);
            argv[1] = path;
            execv(path, argv + 1);
            return 115;
        }
        free(path);

        asprintf(&path, "/%s/save/%s.gz", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/" GAMEBIN, ent->d_name);
            argv[1] = path;
            execv(path, argv + 1);
            return 116;
        }
        free(path);

        asprintf(&path, "/%s/save/%s.bz2", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/" GAMEBIN, ent->d_name);
            argv[1] = path;
            execv(path, argv + 1);
            return 117;
        }
        free(path);
    }

    asprintf(&path, "/" PREFIX "cur/" GAMEBIN);
    argv[1] = path;
    execv(path, argv + 1);
    return 118;
}
