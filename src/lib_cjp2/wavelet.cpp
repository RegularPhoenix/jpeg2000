#include <cstdint>
#include <vector>

#include "codestream.h"
#include "helper.h"

enum Band {
    LL,
    HL,
    LH,
    HH
};

std::vector<std::vector<uint8_t>> idwt(Codestream* codestream) {
    std::vector<uint8_t> spcod = codestream->main_header.get_marker_segment<struct COD>().spcod;

    int lev = bytes_to_u16(spcod[2], spcod[3]);

    while (lev != 0) {
        twod_sr();
        lev--;
    }
}

void twod_sr() {
    twod_interleave();
    hor_sr();
    ver_sr();
}

void twod_interleave() {
}

void hor_sr() {
}

void ver_sr() {
}
