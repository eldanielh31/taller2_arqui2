#!/usr/bin/env bash
set -euo pipefail

# -------------------- Parámetros (sobre-escribibles) --------------------
IMPL="${IMPL:-both}"
MIN="${MIN:-64}"
MAX="${MAX:-1048576}"
COUNT="${COUNT:-50}"
ALPHAS="${ALPHAS:-0,10,20,30,40,50,60,70,80,90,100}"
REPS="${REPS:-20}"          # más repeticiones => menos ruido
WARMUP="${WARMUP:-3}"
MODE="${MODE:-both}"
OUT="${OUT:-data/results.csv}"
SEED="${SEED:-12345}"

# Ejecutar ambas condiciones de alineamiento y combinar resultados
BOTH_ALIGNMENTS="${BOTH_ALIGNMENTS:-1}"  # 1 => corre alineado y desalineado; 0 => solo usa ALIGNED
ALIGNED="${ALIGNED:-1}"                  # usado solo si BOTH_ALIGNMENTS=0
MISALIGN="${MISALIGN:-13}"               # offset para caso desalineado

BIN="./bin/bench"
if [[ ! -x "$BIN" ]]; then
  echo "Error: $BIN not found. Build first with: make ARCH=avx2"
  exit 1
fi

mkdir -p "$(dirname "$OUT")"

run_one () {
  local aligned_flag="$1"
  local outfile="$2"
  "$BIN" \
    --impl "$IMPL" \
    --min "$MIN" \
    --max "$MAX" \
    --count "$COUNT" \
    --alphas "$ALPHAS" \
    --aligned "$aligned_flag" \
    --misalign "$MISALIGN" \
    --reps "$REPS" \
    --warmup "$WARMUP" \
    --mode "$MODE" \
    --outfile "$outfile" \
    --seed "$SEED"
}

if [[ "$BOTH_ALIGNMENTS" -eq 1 ]]; then
  tmp="${OUT}.tmp"
  # 1) alineado -> escribe cabecera
  run_one 1 "$OUT"
  # 2) desalineado -> volcar a tmp y anexar sin cabecera
  run_one 0 "$tmp"
  tail -n +2 "$tmp" >> "$OUT"
  rm -f "$tmp"
  echo "Benchmark done -> $OUT (aligned + misaligned)"
else
  run_one "$ALIGNED" "$OUT"
  echo "Benchmark done -> $OUT"
fi
