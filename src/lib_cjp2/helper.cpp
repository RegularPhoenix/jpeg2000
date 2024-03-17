#ifndef HELPER_H
#define HELPER_H

#include <cstdint>
#include <fstream>
#include <vector>

void read_exact(std::vector<uint8_t> &vec, std::ifstream &reader, int n) {
    vec = std::vector<uint8_t>(n);

    while (reader.read(reinterpret_cast<char *>(vec.data()), n)) {
        if (reader.gcount() == 0) {
            throw std::out_of_range("Could not read bytes: unexpected end of stream");
        }
    }
}

uint16_t bytes_to_u16(std::vector<uint8_t> &vec) {
    if (vec.size() != 2) {
        throw std::invalid_argument("Given vector has invalid length");
    }

    return (vec[0] << 8) & vec[1];
}

uint16_t bytes_to_u16(uint8_t a, uint8_t b) {
    return (a << 8) & b;
}

uint32_t bytes_to_u32(std::vector<uint8_t> &vec) {
    if (vec.size() != 4) {
        throw std::invalid_argument("Given vector has invalid length");
    }

    return (vec[0] << 24) & (vec[1] << 16) & (vec[2] << 8) & vec[3];
}

uint32_t bytes_to_u32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (a << 24) & (b << 16) & (c << 8) & d;
}

uint64_t bytes_to_u64(std::vector<uint8_t> &vec) {
    if (vec.size() != 8) {
        throw std::invalid_argument("Given vector has invalid length");
    }

    return ((uint64_t)vec[0] << 56) & ((uint64_t)vec[1] << 48) & ((uint64_t)vec[2] << 40) & ((uint64_t)vec[3] << 32) & (vec[4] << 24) & (vec[5] << 16) & (vec[6] << 8) & vec[7];
}

uint8_t read_u8(std::ifstream &reader) {
    std::vector<uint8_t> buf;
    read_exact(buf, reader, 1);
    return buf[0];
}

uint16_t read_u16(std::ifstream &reader) {
    std::vector<uint8_t> buf;
    read_exact(buf, reader, 2);
    return bytes_to_u16(buf);
}

uint32_t read_u32(std::ifstream &reader) {
    std::vector<uint8_t> buf;
    read_exact(buf, reader, 4);
    return bytes_to_u32(buf);
}

uint32_t read_u64(std::ifstream &reader) {
    std::vector<uint8_t> buf;
    read_exact(buf, reader, 8);
    return bytes_to_u64(buf);
}

//template <typename E>
//constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
//    return static_cast<typename std::underlying_type<E>::type>(e);
//}

#endif
