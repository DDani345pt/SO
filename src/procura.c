#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../include/utils.h"

// Lê o ficheiro em blocos e constrói linhas até '\n'
#define BUFSZ 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        puterr("Uso: procura <ficheiro> <padrão>");
        return 2;
    }

    const char *fname = argv[1];
    const char *pat = argv[2];

    int fd = open(fname, O_RDONLY);
    if (fd < 0) {
        puterr("Erro: ficheiro não existe ou não pode ser aberto.");
        return 1;
    }

    char buf[BUFSZ];
    char line[8192];
    size_t linelen = 0;
    unsigned long long lineno = 1;
    int found = 0;

    while (1) {
        ssize_t r = read(fd, buf, sizeof(buf));
        if (r == 0) break;
        if (r < 0) {
            if (errno == EINTR) continue;
            puterr("Erro ao ler ficheiro.");
            close(fd);
            return 1;
        }
        for (ssize_t i = 0; i < r; i++) {
            char c = buf[i];
            if (c == '\n') {
                line[linelen] = '\0';
                if (contains_substr(line, pat)) {
                    found = 1;
                    putu64(STDOUT_FILENO, lineno);
                    putstr(STDOUT_FILENO, ": ");
                    putstr(STDOUT_FILENO, line);
                    putnl(STDOUT_FILENO);
                }
                lineno++;
                linelen = 0;
            } else {
                if (linelen + 1 < sizeof(line)) {
                    line[linelen++] = c;
                }
            }
        }
    }
    // última linha sem \n
    if (linelen > 0) {
        line[linelen] = '\0';
        if (contains_substr(line, pat)) {
            found = 1;
            putu64(STDOUT_FILENO, lineno);
            putstr(STDOUT_FILENO, ": ");
            putstr(STDOUT_FILENO, line);
            putnl(STDOUT_FILENO);
        }
    }

    if (!found) {
        puterr("Nenhuma linha encontrada.");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
