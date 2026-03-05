#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "../include/utils.h"

#define BUFSZ 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        puterr("Uso: acrescenta <origem> <destino>");
        return 2;
    }

    int fsrc = open(argv[1], O_RDONLY);
    if (fsrc < 0) {
        puterr("Ficheiro de origem não existe ou não pode ser aberto.");
        return 1;
    }

    int fdst = open(argv[2], O_WRONLY | O_APPEND);
    if (fdst < 0) {
        puterr("Ficheiro de destino não existe ou não pode ser aberto.");
        close(fsrc);
        return 1;
    }

    char buf[BUFSZ];
    while (1) {
        ssize_t r = read(fsrc, buf, sizeof(buf));
        if (r == 0) break;
        if (r < 0) {
            if (errno == EINTR) continue;
            puterr("Erro ao ler origem.");
            close(fsrc);
            close(fdst);
            return 1;
        }
        if (xwrite(fdst, buf, (size_t)r) < 0) {
            puterr("Erro ao escrever destino.");
            close(fsrc);
            close(fdst);
            return 1;
        }
    }

    close(fsrc);
    close(fdst);
    return 0;
}
