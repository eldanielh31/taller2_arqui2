#include "common.hpp"
#include <cstddef>
#include <cstdint>

// Conversión simple (minúsculas 'a'..'z' -> mayúsculas) sin auto-vectorizar.
// Mantén exactamente esta firma para que el linker encuentre el símbolo.
void to_upper_serial(uint8_t* data, std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
        uint8_t c = data[i];
        if (c >= 'a' && c <= 'z') {
            data[i] = static_cast<uint8_t>(c & 0xDFu); // limpia el bit 0x20
        } else {
            data[i] = c;
        }
    }
}
