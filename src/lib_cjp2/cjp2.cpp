#include <cstdint>
#include <fstream>
#include <vector>

#include "jp2.h"

std::vector<uint8_t> decode(std::ifstream &reader) {
    // Read .jp2 file metadata and the codestream within
    JP2 image = JP2::read(reader);

    // Arithmetic decoding

    // Wavelet + DC shifting
}
