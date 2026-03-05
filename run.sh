#!/usr/bin/env bash
DIR="$(cd "$(dirname "$0")" && pwd)"
export PATH="$DIR/bin:$PATH"
exec "$DIR/bin/interpretador"
