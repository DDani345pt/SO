#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stddef.h>

ssize_t xwrite(int fd, const void *buf, size_t n);
void putstr(int fd, const char *s);
void putnl(int fd);
void puterr(const char *s);
void putu64(int fd, unsigned long long v);
void puti64(int fd, long long v);

// lê uma linha de fd até '\n' ou EOF. devolve tamanho (sem '\n'), 0 em EOF, -1 em erro.
ssize_t read_line(int fd, char *buf, size_t max);

// converte permissões para string "rwxr-xr--" (9 chars + '\0')
void mode_to_perm(mode_t mode, char out[10]);

// devolve string textual do tipo de ficheiro (com base em st_mode)
const char* mode_to_type(mode_t mode);

// substring simples (retorna 1 se encontrar)
int contains_substr(const char *hay, const char *needle);

#endif
