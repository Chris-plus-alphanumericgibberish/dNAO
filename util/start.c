#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int lockfd = -1;
struct flock sflock;
long e = 0;

int file_exists(const char *path) {
    struct stat buf;
    int         n   = stat(path, &buf);

    return n == 0;
}

void oops(const char *err, int errnum) {
    if (lockfd != -1) {
        sflock.l_type = F_UNLCK;
        fcntl(lockfd, F_SETLK, &sflock);
        close(lockfd);
    }

    if (errnum)
        printf("error trying to determine game version: %s (%s)\n", err, strerror(errnum));
    else if (e == -1)
        printf("error trying to determine game version: %s (%s)\n", err, strerror(errno));
    else
        printf("error trying to determine game version: %s\n", err);

    getchar();
    exit(1);
}

/* parameters: playground path, player name, arguments to game binary */
int main (int argc, char *argv[]) {
    char        *playground = argv[1];
    char        *player     = argv[2];
    char         path[PATH_MAX];
    char         uid[6];

    if (argc <= 2) {
        printf("Usage: %s <playground> <player> <arguments>\n", argv[0]);
        exit(1);
    }

    size_t  pg_len     = strlen(playground);
    size_t  pl_len     = strlen(player);

    if (pg_len + pl_len + 14 > PATH_MAX)
        oops("command line arguments too long", 0);

    if (playground[pg_len - 1] != '/')
        playground[pg_len++] = '/';

    strncpy(path, playground, pg_len);

    strncpy(path + pg_len, "perm", 5);

    lockfd = open(path, O_RDWR);
    if (lockfd == -1)
        oops("failed to open file 'perm'", errno);

    sflock.l_type = F_WRLCK;
    sflock.l_whence = SEEK_SET;
    sflock.l_start = 0;
    sflock.l_len = 0;

    srand(clock());

    while (fcntl(lockfd, F_SETLK, &sflock) == -1) {
        static unsigned short n1 = 0, n2 = 1;

        if (n2 >= 46368)
            oops("waited too long for lock to be released", 0);

        printf("waiting %hu seconds for lock on 'perm'...\n", n1);
        usleep(n1 * 1000000 + (rand() % 100000));
        n1 = n1 ^ n2;
        n2 = n1 ^ n2;
        n1 = n1 ^ n2;

        n2 += n1;
    }

    strncpy(path + pg_len, "save/", 5);
    int uid_len = snprintf(uid, 6, "%hu", getuid());
    strncpy(path + pg_len + 5, uid, 5);
    strncpy(path + pg_len + 5 + uid_len, player, pl_len);
    strncpy(path + pg_len + 5 + uid_len + pl_len, ".gz", 4);

    if (file_exists(path)) {
        pid_t pid = fork();
        printf("decompressing '%s'...\n", path);
        if (pid == -1)
            oops("could not fork", errno);
        else if (pid)
            waitpid(pid, NULL, 0);
        else
            execl("/bin/gzip", "gzip", "-d", path, (char*) NULL);
    }
    path[pg_len + 5 + uid_len + pl_len] = '\0';

    int  fd  = open(path, O_RDONLY);
    long ver = 0;

    if (fd == -1) {
        strncpy(path + pg_len, uid, 5);
        strncpy(path + pg_len + uid_len, player, pl_len);
        strncpy(path + pg_len + uid_len + pl_len, ".0", 3);
        fd = open(path, O_RDONLY);
        if (fd == -1)
            goto mostrecent;
        lseek(fd, 53, SEEK_SET);
    }

    printf("looking at file '%s'...\n", path);
    if ((e = read(fd, &ver, sizeof(ver))) != sizeof(ver))
        oops("read wrong number of version bytes", 0);

    close(fd);

    sflock.l_type = F_UNLCK;
    if (fcntl(lockfd, F_SETLK, &sflock) == -1)
        oops("failed to release lock on 'perm'", errno);

    close(lockfd);
mostrecent:

    sprintf(path, "dnao-%hhu.%hhu.%hhu-%hhu/dnethack",
            (ver & 0xff000000) >> 24, (ver & 0x00ff0000) >> 16,
            (ver & 0x0000ff00) >> 8,  (ver & 0x000000ff));

    argv[2] = "dnethack";
    execv(path, argv + 2);
    return 0;
}
