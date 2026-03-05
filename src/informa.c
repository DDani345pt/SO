#define _XOPEN_SOURCE 700
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <time.h>
#include "../include/utils.h"

static void print_time(const char *label, time_t t) {
    char buf[64];
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    putstr(STDOUT_FILENO, label);
    putstr(STDOUT_FILENO, buf);
    putnl(STDOUT_FILENO);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        puterr("Uso: informa <ficheiro>");
        return 2;
    }

    struct stat st;
    if (lstat(argv[1], &st) < 0) {
        if (errno == ENOENT) puterr("Erro: ficheiro não existe.");
        else puterr("Erro ao obter informação (stat).");
        return 1;
    }

    char perm[10];
    mode_to_perm(st.st_mode, perm);

    struct passwd *pw = getpwuid(st.st_uid);
    const char *owner = (pw && pw->pw_name) ? pw->pw_name : "(desconhecido)";

    putstr(STDOUT_FILENO, "Tipo: "); putstr(STDOUT_FILENO, mode_to_type(st.st_mode)); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "I-node: "); putu64(STDOUT_FILENO, (unsigned long long)st.st_ino); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "Tamanho (bytes): "); putu64(STDOUT_FILENO, (unsigned long long)st.st_size); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "Dono: "); putstr(STDOUT_FILENO, owner); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "Permissões: "); putstr(STDOUT_FILENO, perm); putnl(STDOUT_FILENO);

    // Datas: acesso (leitura), modificação e mudança de metadata.
    print_time("Acesso: ", st.st_atime);
    print_time("Modificação: ", st.st_mtime);
    print_time("Alteração (metadata): ", st.st_ctime);

    return 0;
}
