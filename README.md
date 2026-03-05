# TP SO 2025/2026 — Mini-sistema de Gestão de Ficheiros

Projeto desenvolvido em **C (Linux)** para a UC de **Sistemas Operativos (2025/2026)**.  
O trabalho consiste em implementar um conjunto de **comandos de manipulação de ficheiros** e um **interpretador de comandos** com suporte a **fork/exec/wait**, **redirecionamentos** e **pipe**, e ainda disponibilizar uma imagem Docker que arranca diretamente no interpretador.  

> ⚠️ Regras do enunciado (muito importante):
> - Implementação com **system calls** (ex.: `open/read/write/close/stat`, `fork/exec/wait`, `pipe`, `dup2`, `opendir/readdir`)  
> - **Proibido** usar `system()`, `fopen/fread/fwrite/fseek`, nem “programação shell” para executar a funcionalidade.  
> - Mensagens de erro devem ser escritas em **stderr**.  

---

## ✅ Funcionalidades

### Parte 1 — Comandos implementados
Cada comando é um executável independente:

- `mostra <ficheiro>` — apresenta todo o conteúdo do ficheiro  
- `copia <origem> <destino>` — cria cópia (não sobrescreve se destino existir)  
- `acrescenta <origem> <destino>` — acrescenta conteúdo ao final do destino  
- `conta [ficheiro]` — conta linhas, palavras e caracteres (tipo `wc`)  
- `apaga <ficheiro>` — remove o ficheiro (avisa se não existir)  
- `informa <ficheiro>` — tipo, inode, tamanho, dono (texto), permissões simbólicas, datas  
- `lista [directoria]` — lista conteúdo e distingue ficheiros vs diretorias  
- `procura <ficheiro> <padrão>` — imprime linhas que contêm o padrão + nº da linha  

### Parte 2 — Interpretador
- Prompt `%` e execução contínua até `termina`  
- Cada comando executa num novo processo (`fork/exec/wait`)  
- Mostra o **código de terminação** ao terminar  
- Suporta:
  - `>` redirecionamento de saída (stdout) para ficheiro (`open` + `dup2`)  
  - `<` redirecionamento de entrada (stdin) a partir de ficheiro (`open` + `dup2`)  
  - **pipe com `\`** entre dois comandos (apenas um pipe, como pedido)  

> Nota: o **pipe** neste TP usa a barra invertida `\` (e não `|` como no bash).

---

## 📦 Requisitos
- Linux (Ubuntu recomendado) com:
  - `gcc`
  - `make`
- Docker (opcional, para a Parte 3)

---

## 🔧 Compilação

Na raiz do projeto:

```bash
make
```

## ▶️ Executar o interpretador (automático)
O
 script run.sh garante que o projeto está compilado e arranca o interpretador com bin/ no PATH para poderes escrever lista, mostra, etc. sem ./bin/.

 ```basj
 ./run.sh
 ```