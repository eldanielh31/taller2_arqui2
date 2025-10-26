#include "common.hpp"
#include <cstdint>
#include <cstddef>

// AVX2: procesa 32 bytes/iteración. Usa load/store ALINEADOS si el puntero lo está;
// de lo contrario, usa loadu/storeu. La cola se resuelve en escalar.
void to_upper_simd(uint8_t* data, std::size_t n) {
    const __m256i a_1 = _mm256_set1_epi8('a' - 1); // para c > 'a' - 1
    const __m256i z1  = _mm256_set1_epi8('z' + 1); // para c < 'z' + 1
    const __m256i bit = _mm256_set1_epi8(0x20);

    std::size_t i = 0;
    const bool is_aligned = (reinterpret_cast<std::uintptr_t>(data) % 32u) == 0;
    const std::size_t vec_end = (n / 32) * 32;

    for (; i < vec_end; i += 32) {
        __m256i v = is_aligned
            ? _mm256_load_si256(reinterpret_cast<const __m256i*>(data + i))
            : _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));

        // máscara 0xFF donde 'a' <= c <= 'z'
        __m256i gt = _mm256_cmpgt_epi8(v, a_1);
        __m256i lt = _mm256_cmpgt_epi8(z1, v);
        __m256i mask = _mm256_and_si256(gt, lt);

        // limpiar bit 0x20 solo donde hay minúscula
        __m256i clear = _mm256_and_si256(mask, bit);
        __m256i res = _mm256_andnot_si256(clear, v);

        if (is_aligned)
            _mm256_store_si256(reinterpret_cast<__m256i*>(data + i), res);
        else
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(data + i), res);
    }

    // Resto
    for (; i < n; ++i) {
        uint8_t c = data[i];
        if (c >= 'a' && c <= 'z') c &= 0xDF;
        data[i] = c;
    }
}
