#pragma once

#include <cstdint>
#include <jpeg2000/utils/num.hpp>
#include <vector>

namespace kimp::jpeg2000::jp2 {

enum EJP2MarkerType : ui16 {
    SOC = 0xff4f, // Start of codestream
    SOT = 0xff90, // Start of tile-part
    SOD = 0xff93, // Start of data
    EOC = 0xffd9, // End of codestream

    SIZ = 0xff51, // Image and tile size

    COD = 0xff52, // Coding style default
    COC = 0xff53, // Coding style component
    RGN = 0xff5e, // Region-of-interest
    QCD = 0xff5c, // Quantization default
    QCC = 0xff5d, // Quantization component
    POC = 0xff5f, // Progression order change

    TLM = 0xff55, // Tile-part lengths
    PLM = 0xff57, // Packet length, main header
    PLT = 0xff58, // Packet length, tile-part header
    PPM = 0xff60, // Packed packet headers, main header
    PPT = 0xff61, // Packed packet headers, tile-part header

    SOP = 0xff91, // Start of packet
    EPH = 0xff92, // End of packet

    CRG = 0xff61, // Component registration
    COM = 0xff64, // Comment
};

class MarkerSegment {
public:
    bool is_initialized;
};

class Siz : public MarkerSegment {
public:
    Siz(std::vector<uint8_t>);
    Siz();

    uint16_t lsiz;
    uint16_t rsiz;
    uint32_t xsiz;
    uint32_t ysiz;
    uint32_t xosiz;
    uint32_t yosiz;
    uint32_t xtsiz;
    uint32_t ytsiz;
    uint32_t xtosiz;
    uint32_t ytosiz;
    uint16_t csiz;
    std::vector<uint8_t> ssiz;
    std::vector<uint8_t> xrsiz;
    std::vector<uint8_t> yrsiz;

    void write(std::vector<uint8_t>&);
};

class Sot : public MarkerSegment {
public:
    Sot(std::vector<ui8>);
    Sot();

    uint16_t lsot;
    uint16_t isot;
    uint32_t psot;
    uint8_t tpsot;
    uint8_t tnsot;

    void write(std::vector<uint8_t>&);
};

class Cod : public MarkerSegment {
public:
    Cod(std::vector<ui8>);
    Cod();

    uint16_t lcod;
    uint8_t scod;
    std::vector<uint8_t> spcod;

    void write(std::vector<uint8_t>&);
};

class Qcd : public MarkerSegment {
public:
    Qcd(std::vector<ui8>);
    Qcd();

    uint16_t lqcd;
    uint8_t sqcd;
    std::vector<uint8_t> spqcd;

    void write(std::vector<uint8_t>&);
};

enum DecodeContext {
    DecodeMainHeader,
    DecodeTilePartHeader,
};

struct MainHeader {
    Siz siz;
    Cod cod;
    Qcd qcd;
};

class TilePart {
public:
    Sot sot;
    std::vector<ui8> bit_stream;
};

class Codestream {
    Codestream(std::vector<ui8>);
    auto write() -> std::vector<uint8_t>;

    MainHeader main_header;
    std::vector<TilePart> tile_parts;
    DecodeContext current_context;
};

} // namespace kimp::jpeg2000::jp2
