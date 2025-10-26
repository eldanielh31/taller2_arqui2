#include "common.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using clock_type = std::chrono::high_resolution_clock;

struct Stat {
    double ns_per_byte{};
    double total_ns{};
    int reps{};
};

template <typename F>
static Stat bench_one(F&& fn, uint8_t* work, const uint8_t* orig, std::size_t n) {
    // Queremos ~64 MiB procesados por punto
    const std::size_t target_bytes = 64ull << 20;
    int reps = std::max<int>(1, static_cast<int>(target_bytes / std::max<std::size_t>(n, 1)));

    // Warm-up
    std::memcpy(work, orig, n);
    fn(work, n);

    // Medición
    double total_ns = 0.0;
    for (int r = 0; r < reps; ++r) {
        std::memcpy(work, orig, n);
        auto t0 = clock_type::now();
        fn(work, n);
        auto t1 = clock_type::now();
        total_ns += std::chrono::duration<double, std::nano>(t1 - t0).count();
    }

    Stat s;
    s.total_ns = total_ns;
    s.reps = reps;
    s.ns_per_byte = total_ns / (static_cast<double>(n) * reps);
    return s;
}

static std::vector<std::size_t> sizes_logspace() {
    std::vector<std::size_t> v;
    // 64 bytes -> 1 MiB aprox., en progresión ~x1.5
    for (std::size_t s = 64; s <= (1u << 20); s = static_cast<std::size_t>(s * 1.5)) {
        v.push_back(s);
        if (s == 0) break;
    }
    if (v.back() != (1u << 20)) v.push_back(1u << 20);
    return v;
}

static void write_header(std::ofstream& out) {
    out << "size,alpha_pct,alignment,impl,mode,ns_per_byte,total_ns,reps\n";
}

int main(int argc, char** argv) {
    // Parámetro -a <porcentaje>
    double alpha = 0.20;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if ((a == "-a" || a == "--alpha") && i + 1 < argc) {
            alpha = std::stod(argv[++i]) / 100.0;
        }
    }
    alpha = std::clamp(alpha, 0.0, 1.0);

    std::ofstream out_al("results_aligned.csv");
    std::ofstream out_mis("results_misaligned.csv");
    write_header(out_al);
    write_header(out_mis);

    auto sizes = sizes_logspace();
    for (std::size_t n : sizes) {
        // Reservas: +64 para poder desalinear 13 bytes sin problemas
        uint8_t* base = aligned_alloc_32(n + 64);
        if (!base) {
            std::cerr << "Fallo al reservar memoria\n";
            return 1;
        }
        uint8_t* aligned_ptr = base;          // 32B alineado
        uint8_t* mis_ptr = base + 13;         // desalineado a propósito

        // Datos originales
        std::vector<uint8_t> orig(n);
        fill_buffer(orig.data(), n, alpha);

        // --- ALIGNED ---
        {
            Stat s_serial = bench_one(to_upper_serial, aligned_ptr, orig.data(), n);
            out_al << n << "," << (alpha * 100.0) << ",aligned,Serial,inplace,"
                   << s_serial.ns_per_byte << "," << s_serial.total_ns << "," << s_serial.reps << "\n";

            Stat s_simd = bench_one(to_upper_simd, aligned_ptr, orig.data(), n);
            out_al << n << "," << (alpha * 100.0) << ",aligned,SIMD,inplace,"
                   << s_simd.ns_per_byte << "," << s_simd.total_ns << "," << s_simd.reps << "\n";
        }

        // --- MISALIGNED ---
        {
            Stat s_serial = bench_one(to_upper_serial, mis_ptr, orig.data(), n);
            out_mis << n << "," << (alpha * 100.0) << ",misaligned,Serial,inplace,"
                    << s_serial.ns_per_byte << "," << s_serial.total_ns << "," << s_serial.reps << "\n";

            Stat s_simd = bench_one(to_upper_simd, mis_ptr, orig.data(), n);
            out_mis << n << "," << (alpha * 100.0) << ",misaligned,SIMD,inplace,"
                    << s_simd.ns_per_byte << "," << s_simd.total_ns << "," << s_simd.reps << "\n";
        }

        aligned_free(base);
    }

    std::cout << "OK: results_aligned.csv y results_misaligned.csv generados.\n";
    return 0;
}
