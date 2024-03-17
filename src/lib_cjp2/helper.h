#include <cstdint>
#include <fstream>
#include <type_traits>
#include <vector>

void read_exact(std::vector<uint8_t> &vec, std::ifstream &reader, int n);

uint16_t bytes_to_u16(std::vector<uint8_t> &vec);
uint16_t bytes_to_u16(uint8_t a, uint8_t b);

uint32_t bytes_to_u32(std::vector<uint8_t> &vec);
uint32_t bytes_to_u32(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

uint64_t bytes_to_u64(std::vector<uint8_t> &vec);

uint16_t read_u8(std::ifstream &reader);
uint16_t read_u16(std::ifstream &reader);
uint16_t read_u32(std::ifstream &reader);
uint16_t read_u64(std::ifstream &reader);

template<typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept;
