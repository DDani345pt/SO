# TP SO 2025/2026 — Mini-sistema de gestão de ficheiros

## Compilar

```bash
make
```

Os executáveis ficam em `bin/`.

## Executar interpretador

```bash
PATH="$PWD/bin:$PATH" ./bin/interpretador
```

Exemplos:

```text
% lista .
% mostra ficheiro.txt > saida.txt
% conta < saida.txt
% lista /tmp \ procura log
% termina
```

> Nota: o pipe pedido no enunciado usa `\` (barra invertida), não `|`.

## Docker

Build:

```bash
docker build -t tp-so .
```

Run:

```bash
docker run -it --rm tp-so
```

Para montar uma pasta do host (para testar ficheiros):

```bash
docker run -it --rm -v "$PWD":/data tp-so
```

Dentro do container podes aceder a `/data`.
