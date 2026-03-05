#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/utils.h"

static void join_path(const char *dir, const char *name, char *out, size_t outsz) {
    size_t dl = strlen(dir);
    size_t nl = strlen(name);
    if (dl + 1 + nl + 1 > outsz) {
        out[0] = '\0';
        return;
    }
    memcpy(out, dir, dl);
    if (dl > 0 && dir[dl-1] != '/') {
        out[dl] = '/';
        memcpy(out+dl+1, name, nl);
        out[dl+1+nl] = '\0';
    } else {
        memcpy(out+dl, name, nl);
        out[dl+nl] = '\0';
    }
}

int main(int argc, char **argv) {
    const char *path = ".";
    if (argc == 2) path = argv[1];
    else if (argc != 1) {
        puterr("Uso: lista [directoria]");
        return 2;
    }

    DIR *d = opendir(path);
    if (!d) {
        puterr("Erro ao abrir diretoria.");
        return 1;
    }

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;

        char full[4096];
        join_path(path, ent->d_name, full, sizeof(full));

        struct stat st;
        if (lstat(full, &st) < 0) {
            // se falhar, imprime só o nome
            putstr(STDOUT_FILENO, "[?] ");
            putstr(STDOUT_FILENO, ent->d_name);
            putnl(STDOUT_FILENO);
            continue;
        }
        if (S_ISDIR(st.st_mode)) putstr(STDOUT_FILENO, "[DIR] ");
        else if (S_ISREG(st.st_mode)) putstr(STDOUT_FILENO, "[FILE] ");
        else if (S_ISLNK(st.st_mode)) putstr(STDOUT_FILENO, "[LINK] ");
        else putstr(STDOUT_FILENO, "[OTHER] ");

        putstr(STDOUT_FILENO, ent->d_name);
        putnl(STDOUT_FILENO);
    }

    closedir(d);
    return 0;
}
