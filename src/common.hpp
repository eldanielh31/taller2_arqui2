#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <immintrin.h>

// Memoria alineada a 32 bytes (para AVX2)
inline uint8_t* aligned_alloc_32(std::size_t n) {
    void* p = nullptr;
    if (posix_memalign(&p, 32, n) != 0) return nullptr;
    return reinterpret_cast<uint8_t*>(p);
}
inline void aligned_free(void* p) { free(p); }

// Generador de datos: 'alpha' = fracción de minúsculas [0..1]
void fill_buffer(uint8_t* buf, std::size_t n, double alpha);

// Implementaciones de conversión ASCII (minúsculas -> mayúsculas, in-place)
void to_upper_serial(uint8_t* data, std::size_t n);
void to_upper_simd(uint8_t* data, std::size_t n);
