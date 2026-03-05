#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../include/utils.h"

#define BUFSZ 4096

int main(int argc, char **argv) {
    if (argc != 3) {
        puterr("Uso: copia <origem> <destino>");
        return 2;
    }

    int fsrc = open(argv[1], O_RDONLY);
    if (fsrc < 0) {
        if (errno == ENOENT) puterr("Ficheiro de origem não existe.");
        else puterr("Erro ao abrir origem.");
        return 1;
    }

    int fdst = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fdst < 0) {
        if (errno == EEXIST) puterr("Ficheiro de destino já existe.");
        else puterr("Erro ao criar destino.");
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
