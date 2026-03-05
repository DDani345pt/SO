#include "utils.h"

#include <errno.h>
#include <string.h>

ssize_t xwrite(int fd, const void *buf, size_t n) {
    const char *p = (const char*)buf;
    size_t left = n;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        left -= (size_t)w;
        p += w;
    }
    return (ssize_t)n;
}

void putstr(int fd, const char *s) {
    if (!s) return;
    xwrite(fd, s, strlen(s));
}

void putnl(int fd) {
    xwrite(fd, "\n", 1);
}

void puterr(const char *s) {
    putstr(STDERR_FILENO, s);
    putnl(STDERR_FILENO);
}

static void putu64_rec(int fd, unsigned long long v) {
    if (v >= 10) putu64_rec(fd, v/10);
    char c = (char)('0' + (v % 10));
    xwrite(fd, &c, 1);
}

void putu64(int fd, unsigned long long v) {
    putu64_rec(fd, v);
}

void puti64(int fd, long long v) {
    if (v < 0) {
        xwrite(fd, "-", 1);
        putu64(fd, (unsigned long long)(-v));
    } else {
        putu64(fd, (unsigned long long)v);
    }
}

ssize_t read_line(int fd, char *buf, size_t max) {
    if (max == 0) return -1;
    size_t i = 0;
    while (i + 1 < max) {
        char c;
        ssize_t r = read(fd, &c, 1);
        if (r == 0) {
            if (i == 0) return 0; // EOF
            break;
        }
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (c == '\n') break;
        buf[i++] = c;
    }
    buf[i] = '\0';
    return (ssize_t)i;
}

void mode_to_perm(mode_t mode, char out[10]) {
    const char chars[] = {'r','w','x'};
    for (int i = 0; i < 9; i++) out[i] = '-';
    out[9] = '\0';
    mode_t masks[9] = {
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
    };
    for (int i = 0; i < 9; i++) {
        if (mode & masks[i]) out[i] = chars[i%3];
    }
}

const char* mode_to_type(mode_t mode) {
    if (S_ISREG(mode)) return "ficheiro normal";
    if (S_ISDIR(mode)) return "diretoria";
    if (S_ISLNK(mode)) return "link simbólico";
    if (S_ISCHR(mode)) return "dispositivo de caracteres";
    if (S_ISBLK(mode)) return "dispositivo de blocos";
    if (S_ISFIFO(mode)) return "FIFO";
    if (S_ISSOCK(mode)) return "socket";
    return "desconhecido";
}

int contains_substr(const char *hay, const char *needle) {
    if (!needle || !*needle) return 1;
    if (!hay) return 0;
    size_t nlen = strlen(needle);
    for (const char *p = hay; *p; ++p) {
        if (*p == *needle) {
            if (strncmp(p, needle, nlen) == 0) return 1;
        }
    }
    return 0;
}
