#!/usr/bin/env bash
# Normaliza marcas de tiempo si hay archivos "en el futuro" respecto al reloj del sistema.
set -euo pipefail

# Detecta si existe al menos un archivo con mtime > now y, de ser así, hace touch a todos.
now_epoch="$(date +%s)"
need_fix=0

while IFS= read -r -d '' f; do
  # GNU stat (Linux). Fallback a BSD stat (macOS) si falla.
  if mt=$(stat -c %Y "$f" 2>/dev/null); then
    :
  else
    mt=$(stat -f %m "$f")
  fi
  if [[ "$mt" -gt "$now_epoch" ]]; then
    need_fix=1
    break
  fi
done < <(find . -type f -print0)

if [[ "$need_fix" -eq 1 ]]; then
  echo "Clock skew detected: touching all files to current time..."
  find . -type f -exec touch {} +
else
  echo "No clock skew detected."
fi
