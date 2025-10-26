# Taller 02 — Ejecución rápida

Este README contiene **solo** lo necesario para compilar, ejecutar el benchmark y generar la figura con `make`.

---

## 1) Requisitos mínimos

- GNU/Linux (no virtualizado) con **bash**.
- **g++** (GCC) con soporte C++.
- **Python 3.8+** con los paquetes:
  - `matplotlib`, `pandas`, `numpy`  
  (si no los tiene, instálelos: `python3 -m pip install --user matplotlib pandas numpy`)

> No se requieren permisos especiales ni ejecutar scripts manualmente; el `Makefile` orquesta todo.

---

## 2) Comandos esenciales

- **Compilar**
  ```bash
  make
  ```
  Genera el ejecutable `bin/bench`.

- **Compilar + correr benchmark + graficar**
  ```bash
  make run
  ```
  1. Compila (si es necesario).
  2. Ejecuta `scripts/run_bench.sh` → crea `data/results.csv`.
  3. Ejecuta `scripts/plot_results.py` → crea `plots/figura2.png`.

- **Limpiar objetos y binario**
  ```bash
  make clean
  ```
---

## 3) Variables (opcionales) que puede ajustar al invocar `make`

> Si no las especifica, se usan los valores por defecto del `Makefile`.

- **ALPHA**: porcentaje de caracteres alfabéticos usado en el título de la figura (e.g., 20).
- **CSV**: ruta del CSV a graficar (por defecto `data/results.csv`).
- **PLOT_OUT**: ruta de salida de la imagen (por defecto `plots/figura2.png`).

Ejemplos:
```bash
make run ALPHA=30
make run CSV=data/results.csv PLOT_OUT=plots/fig2_alt.png
```

También puede sobreescribir parámetros de compilación si lo requiere:
```bash
make CXXSTD=-std=c++17
make CXXFLAGS="-O3 -march=native -DNDEBUG"
```

---

## 4) Archivos generados

- `bin/bench` — ejecutable del benchmark.
- `results_*.csv` — resultados crudos (*columns*: `size, alpha_pct, alignment, impl, mode, ns_per_byte, total_ns, reps`).
- `figura2.png` — figura producida por `scripts/plot_results.py`.
- `build/*.o` — objetos intermedios (se eliminan con `make clean`).

