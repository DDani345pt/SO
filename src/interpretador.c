#define _GNU_SOURCE
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "../include/utils.h"

#define MAXTOK 64
#define MAXLINE 4096

static int split_tokens(char *line, char *argv[], int max) {
    int n = 0;
    // separa por espaços e tabs
    char *p = line;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;
        if (n >= max-1) break;
        argv[n++] = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) { *p = '\0'; p++; }
    }
    argv[n] = NULL;
    return n;
}

static void print_exit_status(const char *prefix, const char *cmd, int status) {
    if (WIFEXITED(status)) {
        putstr(STDOUT_FILENO, prefix);
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        putstr(STDOUT_FILENO, " com código ");
        puti64(STDOUT_FILENO, (long long)WEXITSTATUS(status));
        putnl(STDOUT_FILENO);
    } else if (WIFSIGNALED(status)) {
        putstr(STDOUT_FILENO, prefix);
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        putstr(STDOUT_FILENO, " terminado por sinal ");
        puti64(STDOUT_FILENO, (long long)WTERMSIG(status));
        putnl(STDOUT_FILENO);
    } else {
        putstr(STDOUT_FILENO, prefix);
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        if (cmd) { putstr(STDOUT_FILENO, " "); putstr(STDOUT_FILENO, cmd); }
        putstr(STDOUT_FILENO, " (estado desconhecido)");
        putnl(STDOUT_FILENO);
    }
}

static int run_simple(char *argv[]) {
    pid_t pid = fork();
    if (pid < 0) { puterr("fork falhou"); return 1; }
    if (pid == 0) {
        execvp(argv[0], argv);
        // se exec falhar
        puterr("Erro: exec falhou (comando não encontrado?)");
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) { puterr("waitpid falhou"); return 1; }
    print_exit_status("Terminou comando", argv[0], status);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int run_redirect_out(char *argv[], const char *outfile) {
    pid_t pid = fork();
    if (pid < 0) { puterr("fork falhou"); return 1; }
    if (pid == 0) {
        int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) { puterr("Erro ao abrir ficheiro de saída"); _exit(1); }
        if (dup2(fd, STDOUT_FILENO) < 0) { puterr("dup2 falhou"); _exit(1); }
        close(fd);
        execvp(argv[0], argv);
        puterr("Erro: exec falhou");
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) { puterr("waitpid falhou"); return 1; }
    print_exit_status("Terminou comando", argv[0], status);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int run_redirect_in(char *argv[], const char *infile) {
    pid_t pid = fork();
    if (pid < 0) { puterr("fork falhou"); return 1; }
    if (pid == 0) {
        int fd = open(infile, O_RDONLY);
        if (fd < 0) { puterr("Erro ao abrir ficheiro de entrada"); _exit(1); }
        if (dup2(fd, STDIN_FILENO) < 0) { puterr("dup2 falhou"); _exit(1); }
        close(fd);
        execvp(argv[0], argv);
        puterr("Erro: exec falhou");
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) { puterr("waitpid falhou"); return 1; }
    print_exit_status("Terminou comando", argv[0], status);
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int run_pipe(char *argv1[], char *argv2[]) {
    int pfd[2];
    if (pipe(pfd) < 0) { puterr("pipe falhou"); return 1; }

    pid_t p1 = fork();
    if (p1 < 0) { puterr("fork falhou"); return 1; }
    if (p1 == 0) {
        // stdout -> pipe write
        close(pfd[0]);
        if (dup2(pfd[1], STDOUT_FILENO) < 0) { puterr("dup2 falhou"); _exit(1); }
        close(pfd[1]);
        execvp(argv1[0], argv1);
        puterr("Erro: exec 1 falhou");
        _exit(127);
    }

    pid_t p2 = fork();
    if (p2 < 0) { puterr("fork falhou"); return 1; }
    if (p2 == 0) {
        // stdin <- pipe read
        close(pfd[1]);
        if (dup2(pfd[0], STDIN_FILENO) < 0) { puterr("dup2 falhou"); _exit(1); }
        close(pfd[0]);
        execvp(argv2[0], argv2);
        puterr("Erro: exec 2 falhou");
        _exit(127);
    }

    close(pfd[0]);
    close(pfd[1]);

    int st1, st2;
    waitpid(p1, &st1, 0);
    waitpid(p2, &st2, 0);

    // o resultado mostrado deve ser do último comando
    print_exit_status("Terminou pipe", argv2[0], st2);
    return WIFEXITED(st2) ? WEXITSTATUS(st2) : 1;
}

int main(void) {
    char line[MAXLINE];

    while (1) {
        putstr(STDOUT_FILENO, "% ");
        ssize_t n = read_line(STDIN_FILENO, line, sizeof(line));
        if (n == 0) break; // EOF
        if (n < 0) { puterr("Erro ao ler comando"); continue; }

        // linha vazia
        if (line[0] == '\0') continue;

        if (strcmp(line, "termina") == 0) {
            break;
        }

        // deteta pipe "\\" (um único) com espaços opcionais
        char *pipepos = strstr(line, "\\");
        if (pipepos) {
            *pipepos = '\0';
            char *right = pipepos + 1;
            while (*right == ' ' || *right == '\t') right++;

            // trim do lado esquerdo (fim)
            char *end = line + strlen(line);
            while (end > line && (end[-1] == ' ' || end[-1] == '\t')) { end[-1] = '\0'; end--; }

            char *argv1[MAXTOK];
            char *argv2[MAXTOK];
            int n1 = split_tokens(line, argv1, MAXTOK);
            int n2 = split_tokens(right, argv2, MAXTOK);
            if (n1 == 0 || n2 == 0) {
                puterr("Pipe inválido.");
                continue;
            }
            run_pipe(argv1, argv2);
            continue;
        }

        // redirecionamentos
        char *gt = strchr(line, '>');
        char *lt = strchr(line, '<');
        if (gt) {
            *gt = '\0';
            char *outfile = gt + 1;
            while (*outfile == ' ' || *outfile == '\t') outfile++;
            // remove espaços finais do cmd
            char *end = line + strlen(line);
            while (end > line && (end[-1] == ' ' || end[-1] == '\t')) { end[-1] = '\0'; end--; }

            char *argv[MAXTOK];
            int na = split_tokens(line, argv, MAXTOK);
            if (na == 0 || outfile[0] == '\0') {
                puterr("Redirecionamento > inválido.");
                continue;
            }
            // outfile pode ter espaços -> pega no primeiro token
            char outcopy[MAXLINE];
            strncpy(outcopy, outfile, sizeof(outcopy)-1);
            outcopy[sizeof(outcopy)-1] = '\0';
            char *outv[MAXTOK];
            split_tokens(outcopy, outv, MAXTOK);
            run_redirect_out(argv, outv[0]);
            continue;
        }
        if (lt) {
            *lt = '\0';
            char *infile = lt + 1;
            while (*infile == ' ' || *infile == '\t') infile++;
            char *end = line + strlen(line);
            while (end > line && (end[-1] == ' ' || end[-1] == '\t')) { end[-1] = '\0'; end--; }

            char *argv[MAXTOK];
            int na = split_tokens(line, argv, MAXTOK);
            if (na == 0 || infile[0] == '\0') {
                puterr("Redirecionamento < inválido.");
                continue;
            }
            char incopy[MAXLINE];
            strncpy(incopy, infile, sizeof(incopy)-1);
            incopy[sizeof(incopy)-1] = '\0';
            char *inv[MAXTOK];
            split_tokens(incopy, inv, MAXTOK);
            run_redirect_in(argv, inv[0]);
            continue;
        }

        // comando normal
        char *argv[MAXTOK];
        int na = split_tokens(line, argv, MAXTOK);
        if (na == 0) continue;
        run_simple(argv);
    }

    return 0;
}
