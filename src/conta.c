#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "../include/utils.h"

#define BUFSZ 4096

static int process_fd(int fd, unsigned long long *lines, unsigned long long *words, unsigned long long *chars) {
    char buf[BUFSZ];
    int in_word = 0;
    while (1) {
        ssize_t r = read(fd, buf, sizeof(buf));
        if (r == 0) break;
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        for (ssize_t i = 0; i < r; i++) {
            unsigned char c = (unsigned char)buf[i];
            (*chars)++;
            if (c == '\n') (*lines)++;
            if (isspace(c)) {
                in_word = 0;
            } else {
                if (!in_word) {
                    (*words)++;
                    in_word = 1;
                }
            }
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    int fd = STDIN_FILENO;
    if (argc == 2) {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            puterr("Erro: ficheiro não existe ou não pode ser aberto.");
            return 1;
        }
    } else if (argc != 1) {
        puterr("Uso: conta [ficheiro]  (ou usar redirecionamento: conta < dados.txt)");
        return 2;
    }

    unsigned long long lines=0, words=0, chars=0;
    if (process_fd(fd, &lines, &words, &chars) < 0) {
        puterr("Erro ao ler.");
        if (fd != STDIN_FILENO) close(fd);
        return 1;
    }
    if (fd != STDIN_FILENO) close(fd);

    putstr(STDOUT_FILENO, "Linhas: "); putu64(STDOUT_FILENO, lines); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "Palavras: "); putu64(STDOUT_FILENO, words); putnl(STDOUT_FILENO);
    putstr(STDOUT_FILENO, "Caracteres: "); putu64(STDOUT_FILENO, chars); putnl(STDOUT_FILENO);

    return 0;
}
