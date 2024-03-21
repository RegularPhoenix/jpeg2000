#ifndef CODESTREAM_H
#define CODESTREAM_H

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "helper.h"

struct MarkerSegment;
typedef struct MarkerSegment MarkerSegment;

struct TilePartHeader {
    std::vector<MarkerSegment> segments;

    template <typename T>
    bool is_marker_present();
};

typedef struct TilePart {
    TilePartHeader header;
    std::vector<uint8_t> bit_stream;
} TilePart;

typedef struct MainHeader {
    std::vector<MarkerSegment> segments;

    template <typename T>
    bool is_marker_present();

    template <typename T>
    T get_marker_segment();
} MainHeader;

enum class Context {
    MainHeader,
    TilePart,
    InBitStream
};

void ensure_context(Context context);

struct Codestream {
    MainHeader main_header;
    std::vector<TilePart> tile_parts;
    Context context;
   private:
    bool read_segment(const std::vector<uint8_t>& data, size_t& byte_ind, uint16_t marker);
   public:
    Codestream(const std::vector<uint8_t>& data);
    void ensure_context(Context context);
};

// Markers and segments
//
// Required:
// SOC
// SOT
// SOD
// EOC
// SIZ
// COD
// QCD
//
// Optional:
// COC
// RGN
// QCC
// POD
// TLM
// PLM
// PLT
// PPM
// PPT
// SOP
// EPH
// CME

enum class Marker {
    SOC = 0xFF4F,
    SOT = 0xFF90,
    SOD = 0xFF93,
    EOC = 0xFFD9,
    SIZ = 0xFF51,
    COD = 0xFF52,
    COC = 0xFF53,
    RGN = 0xFF5E,
    QCD = 0xFF5C,
    QCC = 0xFF5D,
    POD = 0xFF5F,
    TLM = 0xFF55,
    PLM = 0xFF57,
    PLT = 0xFF58,
    PPM = 0xFF60,
    PPT = 0xFF61,
    SOP = 0xFF91,
    EPH = 0xFF92,
    CME = 0xFF64,
};

struct MarkerSegment {
    virtual int len() { return 0; }
};

struct SOT : MarkerSegment {
    SOT(uint16_t lsot, uint16_t isot, uint32_t psot, uint8_t tpsot, uint8_t tnsot) : lsot(lsot), isot(isot), psot(psot), tpsot(tpsot), tnsot(tnsot) {}

    uint16_t lsot;
    uint16_t isot;
    uint32_t psot;
    uint8_t tpsot;
    uint8_t tnsot;

    static void read_to_codestream(Codestream &codestream, const std::vector<uint8_t>& data, size_t& byte_ind) {
        uint16_t len = bytes_to_u16(data[byte_ind], data[byte_ind+1]);
        byte_ind += 2;

        if(byte_ind + len >= data.size())
            throw std::out_of_range("Could not read bytes: unexpected end of stream");
        std::vector<uint8_t> bytes(data.begin() + byte_ind, data.begin() + byte_ind + len);
        byte_ind += len;
        // read_exact(bytes, reader, len);

        codestream.ensure_context(Context::TilePart);

        if (codestream.main_header.is_marker_present<SOT>()) {
            throw std::runtime_error("Segment is used twice!");
        }

        codestream.tile_parts.push_back(TilePart());

        codestream.tile_parts[codestream.tile_parts.size() - 1].header.segments.push_back(SOT(
            len,
            bytes_to_u16(bytes[0], bytes[1]),
            bytes_to_u32(bytes[2], bytes[3], bytes[4], bytes[5]),
            bytes[6],
            bytes[7]));
    }
};

struct SIZ : MarkerSegment {
    SIZ(uint16_t lsiz, uint16_t rsiz, uint32_t xsiz, uint32_t ysiz, uint32_t xosiz, uint32_t yosiz, uint32_t xtsiz, uint32_t ytsiz, uint32_t xtosiz, uint32_t ytosiz, uint16_t csiz, std::vector<uint8_t> ssiz, std::vector<uint8_t> xrsiz, std::vector<uint8_t> yrsiz) : lsiz(lsiz), rsiz(rsiz), xsiz(xsiz), ysiz(ysiz), xosiz(xosiz), yosiz(yosiz), xtsiz(xtsiz), ytsiz(ytsiz), xtosiz(xtosiz), ytosiz(ytosiz), csiz(csiz), ssiz(std::move(ssiz)), xrsiz(std::move(xrsiz)), yrsiz(std::move(yrsiz)) {}

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

    static void read_to_codestream(Codestream &codestream, const std::vector<uint8_t>& data, size_t& byte_ind) {
        uint16_t len = bytes_to_u16(data[byte_ind], data[byte_ind+1]);
        byte_ind += 2;

        if (len > 49191) {
            throw std::runtime_error("Invalid len");
        }
        
        // read_exact(bytes, reader, len);
        if(byte_ind + len >= data.size())
            throw std::out_of_range("Could not read bytes: unexpected end of stream");
        std::vector<uint8_t> bytes(data.begin() + byte_ind, data.begin() + byte_ind + len);
        byte_ind += len;

        codestream.ensure_context(Context::MainHeader);

        if (codestream.main_header.is_marker_present<SIZ>()) {
            throw std::runtime_error("Segment is used twice!");
        }

        SIZ res = SIZ(
            len,
            bytes_to_u16(bytes[0], bytes[1]),
            bytes_to_u32(bytes[2], bytes[3], bytes[4], bytes[5]),
            bytes_to_u32(bytes[6], bytes[7], bytes[8], bytes[9]),
            bytes_to_u32(bytes[10], bytes[11], bytes[12], bytes[13]),
            bytes_to_u32(bytes[14], bytes[15], bytes[16], bytes[17]),
            bytes_to_u32(bytes[18], bytes[19], bytes[20], bytes[21]),
            bytes_to_u32(bytes[22], bytes[23], bytes[24], bytes[25]),
            bytes_to_u32(bytes[26], bytes[27], bytes[28], bytes[29]),
            bytes_to_u32(bytes[30], bytes[31], bytes[32], bytes[33]),
            bytes_to_u16(bytes[34], bytes[35]),
            std::vector<uint8_t>(),
            std::vector<uint8_t>(),
            std::vector<uint8_t>());

        codestream.main_header.segments.push_back(res);
    }
};

struct COD : MarkerSegment {
    uint16_t lcod;
    uint8_t scod;
    std::vector<uint8_t> spcod;
};

#endif
