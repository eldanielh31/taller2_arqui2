import sys
import math
import pandas as pd
import matplotlib.pyplot as plt

# Uso: python3 scripts/plot_results.py results_aligned.csv results_misaligned.csv
if len(sys.argv) < 3:
    print("Uso: plot_results.py aligned.csv misaligned.csv")
    sys.exit(1)

def load_and_normalize(path):
    df = pd.read_csv(path)
    # pivot para obtener ns/byte por impl
    piv = df.pivot_table(index="size", columns="impl", values="ns_per_byte", aggfunc="min")
    # normalizar contra Serial
    piv["norm"] = piv["SIMD"] / piv["Serial"]
    return piv.reset_index()[["size", "norm"]]

aligned = load_and_normalize(sys.argv[1])
misaligned = load_and_normalize(sys.argv[2])

plt.figure(figsize=(12,6))
plt.plot(aligned["size"], aligned["norm"], marker="o", label="SIMD (aligned)")
plt.plot(misaligned["size"], misaligned["norm"], marker="o", label="SIMD (misaligned)")
plt.axhline(1.0, linestyle="--", linewidth=1.0, color="#888888")

plt.xscale("log")
plt.xlabel("Tamaño de cadena (bytes)")
plt.ylabel("Tiempo normalizado (Serial = 1.0) — valores < 1 son mejores")

# título con alpha si ambos ficheros comparten el mismo
# (leemos del primero)
df0 = pd.read_csv(sys.argv[1])
alpha = df0["alpha_pct"].iloc[0] if "alpha_pct" in df0.columns else 0
plt.title(f"Conversión ASCII (alpha={int(alpha)}%) — SIMD vs Serial")

plt.legend()
plt.grid(True, which="both", linestyle=":", linewidth=0.6, alpha=0.6)
plt.tight_layout()
plt.savefig("figura2.png", dpi=180)
print("Gráfico guardado en figura2.png")
