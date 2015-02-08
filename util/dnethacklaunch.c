#define _BSD_SOURCE
#define _GNU_SOURCE

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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

        if (strncmp(ent->d_name, "dnao-", strlen("dnao-")))
            continue;

        asprintf(&path, "/%s/1337%s.0", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            printf("game in progress: %s", path);
            return 113;
        }
        free(path);

        asprintf(&path, "/%s/save/1337%s.e", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            printf("save error: %s", path);
            return 114;
        }
        free(path);

        asprintf(&path, "/%s/save/1337%s", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/dnethack", ent->d_name);
            execl(path, path, "-u", argv[1], NULL);
            return 115;
        }
        free(path);

        asprintf(&path, "/%s/save/1337%s.gz", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/dnethack", ent->d_name);
            execl(path, path, "-u", argv[1], NULL);
            return 116;
        }
        free(path);

        asprintf(&path, "/%s/save/1337%s.bz2", ent->d_name, argv[1]);
        if (!access(path, F_OK)) {
            free(path);
            asprintf(&path, "/%s/dnethack", ent->d_name);
            execl(path, path, "-u", argv[1], NULL);
            return 117;
        }
        free(path);
    }

    asprintf(&path, "/dnao-0.0.0-0/dnethack");
    execl(path, path, "-u", argv[1], NULL);
    return 118;
}
