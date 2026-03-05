#include <unistd.h>
#include <errno.h>
#include "../include/utils.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        puterr("Uso: apaga <ficheiro>");
        return 2;
    }

    if (unlink(argv[1]) < 0) {
        if (errno == ENOENT) {
            puterr("Aviso: ficheiro não existe.");
            return 1;
        }
        puterr("Erro ao apagar ficheiro.");
        return 1;
    }

    return 0;
}
