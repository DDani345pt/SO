CC=gcc
CFLAGS=-Wall -Wextra -O2
INCLUDES=-Iinclude

BINDIR=bin
SRCDIR=src

UTILS_OBJ=$(BINDIR)/utils.o

CMDS=mostra copia acrescenta conta apaga informa lista procura

all: dirs $(CMDS) interpretador

dirs:
	@mkdir -p $(BINDIR)

$(UTILS_OBJ): $(SRCDIR)/utils.c include/utils.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BINDIR)/%.o: $(SRCDIR)/%.c include/utils.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

%: $(BINDIR)/%.o $(UTILS_OBJ)
	$(CC) $(CFLAGS) $^ -o $(BINDIR)/$@

interpretador: $(BINDIR)/interpretador.o $(UTILS_OBJ)
	$(CC) $(CFLAGS) $^ -o $(BINDIR)/interpretador

clean:
	rm -rf $(BINDIR)

.PHONY: all clean dirs
