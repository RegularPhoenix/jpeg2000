#include <cstdint>
#include <fstream>
#include <vector>

#include "codestream.h"

struct Box {
    uint32_t lbox;
    uint32_t tbox;
    uint64_t xlbox;

    std::ifstream *reader;
    int dbox_start;
    // std::vector<uint8_t> dbox;

    static Box read(std::ifstream &reader);

    template <typename T>
    T unbox();
};

struct Profile {
    uint32_t brand;
    std::vector<uint32_t> compatibility_list;
};

struct ImageHeader {
    uint16_t vers;
    uint16_t nc;
    uint32_t height;
    uint32_t width;
    uint8_t bpc;
    uint8_t c;
    uint8_t unk;
    uint8_t ipr;
};

struct ColourSpecification {
    uint8_t meth;
    uint8_t prec;
    uint8_t approx;
    uint32_t enum_cs;
    // Profile?
};

struct JP2 {
    Profile profile_box;
    ImageHeader image_header;
    ColourSpecification colour_specification;
    Codestream codestream;

    static JP2 read(std::ifstream &reader);
};
