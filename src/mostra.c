#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../include/utils.h"

#define BUFSZ 4096

int main(int argc, char **argv) {
    if (argc != 2) {
        puterr("Uso: mostra <ficheiro>");
        return 2;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        if (errno == ENOENT) {
            puterr("O ficheiro não existe.");
        } else {
            puterr("Erro ao abrir ficheiro.");
        }
        return 1;
    }

    char buf[BUFSZ];
    while (1) {
        ssize_t r = read(fd, buf, sizeof(buf));
        if (r == 0) break;
        if (r < 0) {
            if (errno == EINTR) continue;
            puterr("Erro ao ler ficheiro.");
            close(fd);
            return 1;
        }
        if (xwrite(STDOUT_FILENO, buf, (size_t)r) < 0) {
            puterr("Erro ao escrever em stdout.");
            close(fd);
            return 1;
        }
    }
    close(fd);
    return 0;
}
