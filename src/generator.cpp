#include "common.hpp"
#include <random>

// Llena 'buf' con ASCII. Con prob. 'alpha' genera minúscula ('a'..'z');
// si no, genera otro ASCII no minúscula (mayúsculas, dígitos, símbolos...).
void fill_buffer(uint8_t* buf, std::size_t n, double alpha) {
    std::mt19937_64 rng(0xC0FFEE1234ULL);
    std::uniform_real_distribution<double> prob(0.0, 1.0);
    std::uniform_int_distribution<int> dist_lower(0, 25);
    std::uniform_int_distribution<int> dist_ascii(0, 127);

    for (std::size_t i = 0; i < n; ++i) {
        if (prob(rng) < alpha) {
            buf[i] = static_cast<uint8_t>('a' + dist_lower(rng));
        } else {
            uint8_t x = static_cast<uint8_t>(dist_ascii(rng));
            if (x >= 'a' && x <= 'z') x &= 0xDF;  // evita minúsculas fuera del alpha
            buf[i] = x;
        }
    }
}
