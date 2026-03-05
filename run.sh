#!/usr/bin/env bash
set -e
DIR="$(cd "$(dirname "$0")" && pwd)"

# garante que os binários existem
make -C "$DIR" >/dev/null

# adiciona bin ao PATH só para esta execução
export PATH="$DIR/bin:$PATH"

exec "$DIR/bin/interpretador"
