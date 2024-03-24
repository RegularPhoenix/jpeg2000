#include <cstdint>
#include <jpeg2000/jp2/codestream.hpp>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace kimp::jpeg2000::jp2 {

uint16_t bytes_to_u16(uint8_t a, uint8_t b) {
    return (a << 8) & b;
}

std::vector<uint8_t> u16_to_bytes(uint16_t i) {
    return std::vector<uint8_t>{(uint8_t) (i >> 8), (uint8_t) i};
}

uint32_t bytes_to_u32(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
    return (a << 24) & (b << 16) & (c << 8) & d;
}

std::vector<uint8_t> u32_to_bytes(uint32_t i) {
    return std::vector<uint8_t>{(uint8_t) (i >> 24), (uint8_t) (i >> 16 & 0xFF), (uint8_t) (i >> 8), (uint8_t) i};
}

void push_vec(std::vector<uint8_t> & to, std::vector<uint8_t> from) {
    to.insert(to.end(), from.begin(), from.end());
}
Siz::Siz() {}
Siz::Siz(std::vector<uint8_t> bytes) {
    uint16_t len = bytes_to_u16(bytes[0], bytes[1]);

    if (len > 49191) {
        throw std::runtime_error("Invalid len");
    }

    lsiz = len;
    rsiz = bytes_to_u32(bytes[2], bytes[3], bytes[4], bytes[5]);
    xsiz = bytes_to_u32(bytes[6], bytes[7], bytes[8], bytes[9]);
    ysiz = bytes_to_u32(bytes[10], bytes[11], bytes[12], bytes[13]);
    xosiz = bytes_to_u32(bytes[14], bytes[15], bytes[16], bytes[17]);
    yosiz = bytes_to_u32(bytes[18], bytes[19], bytes[20], bytes[21]);
    xtsiz = bytes_to_u32(bytes[22], bytes[23], bytes[24], bytes[25]);
    ytsiz = bytes_to_u32(bytes[26], bytes[27], bytes[28], bytes[29]);
    xtosiz = bytes_to_u32(bytes[30], bytes[31], bytes[32], bytes[33]);
    ytosiz = bytes_to_u16(bytes[34], bytes[35]);
    ssiz = std::vector<uint8_t>();
    xrsiz = std::vector<uint8_t>();
    yrsiz = std::vector<uint8_t>();

    auto st_index = 36;
    for (int i = 0; i < (len - 36); i += 3) {
        ssiz.push_back(bytes[st_index + i]);
        xrsiz.push_back(bytes[st_index + i + 1]);
        yrsiz.push_back(bytes[st_index + i + 2]);
    }
}

auto Siz::write(std::vector<uint8_t> & bytes) -> void {
    push_vec(bytes, u16_to_bytes(EJP2MarkerType::SIZ));
    push_vec(bytes, u16_to_bytes(lsiz));
    push_vec(bytes, u32_to_bytes(rsiz));
    push_vec(bytes, u32_to_bytes(xsiz));
    push_vec(bytes, u32_to_bytes(ysiz));
    push_vec(bytes, u32_to_bytes(xosiz));
    push_vec(bytes, u32_to_bytes(yosiz));
    push_vec(bytes, u32_to_bytes(xtsiz));
    push_vec(bytes, u32_to_bytes(ytsiz));
    push_vec(bytes, u32_to_bytes(xtosiz));
    push_vec(bytes, u32_to_bytes(ytosiz));

    for (int i = 0; i < ssiz.size(); i++) {
        bytes.push_back(ssiz[i]);
        bytes.push_back(xrsiz[i]);
        bytes.push_back(yrsiz[i]);
    }
}

Sot::Sot() {}
Sot::Sot(std::vector<uint8_t> bytes) {
    uint16_t len = bytes_to_u16(bytes[0], bytes[1]);

    lsot = len;
    isot = bytes_to_u16(bytes[2], bytes[3]);
    psot = bytes_to_u32(bytes[4], bytes[5], bytes[6], bytes[7]);
    tpsot = bytes[8];
    tnsot = bytes[9];
}

auto Sot::write(std::vector<uint8_t> & bytes) -> void {
    push_vec(bytes, u16_to_bytes(EJP2MarkerType::SOT));
    push_vec(bytes, u16_to_bytes(lsot));
    push_vec(bytes, u32_to_bytes(psot));
    bytes.push_back(tpsot);
    bytes.push_back(tnsot);
}

Cod::Cod() {}
Cod::Cod(std::vector<uint8_t> bytes) {
    uint16_t len = bytes_to_u16(bytes[0], bytes[1]);

    lcod = len;
    scod = bytes[2];
    spcod = std::vector<uint8_t>();
    spcod.assign(bytes.begin() + 2, bytes.end());
}

auto Cod::write(std::vector<uint8_t> & bytes) -> void {
    push_vec(bytes, u16_to_bytes(EJP2MarkerType::COD));
    push_vec(bytes, u16_to_bytes(lcod));
    bytes.push_back(scod);
    push_vec(bytes, spcod);
}

Qcd::Qcd() {}
Qcd::Qcd(std::vector<uint8_t> bytes) {
    uint16_t len = bytes_to_u16(bytes[0], bytes[1]);

    lqcd = len;
    sqcd = bytes[2];
    spqcd = std::vector<uint8_t>();
    spqcd.assign(bytes.begin() + 2, bytes.end());
}

auto Qcd::write(std::vector<uint8_t> & bytes) -> void {
    push_vec(bytes, u16_to_bytes(EJP2MarkerType::QCD));
    push_vec(bytes, u16_to_bytes(lqcd));
    bytes.push_back(sqcd);
    push_vec(bytes, spqcd);
}

auto read_marker(std::vector<uint8_t> & bytes, int & index) -> uint16_t {
    return bytes_to_u16(bytes.at(index++), bytes.at(index++));
}

auto read_segment(std::vector<uint8_t> & bytes, int & index) -> std::tuple<uint16_t, uint16_t> {
    uint16_t marker = read_marker(bytes, index);
    uint16_t len = bytes_to_u16(bytes.at(index + 1), bytes.at(index + 2));
    return { marker, len };
}

Codestream::Codestream(std::vector<uint8_t> bytes) {
    int index = 0;
    if (read_marker(bytes, index) != EJP2MarkerType::SOC) {
        throw std::runtime_error("SOC expected");
    }

    while (index < bytes.size()) {
        auto t = read_segment(bytes, index);
        std::vector<uint8_t> buf;

        auto len = std::get<1>(t);
        buf.assign(bytes.begin() + index, bytes.begin() + index + len);

        switch (std::get<0>(t)) {
            case EJP2MarkerType::SIZ:
                if (main_header.siz.is_initialized)
                    throw std::runtime_error("Siz is already present");
                main_header.siz = Siz(buf);
                break;

            case EJP2MarkerType::SOT:
                tile_parts.push_back(TilePart());

                if (tile_parts[tile_parts.size() - 1].sot.is_initialized)
                    throw std::runtime_error("Sot is already present");
                tile_parts[tile_parts.size() - 1].sot = Sot(buf);
                break;

            case EJP2MarkerType::COD:
                if (main_header.cod.is_initialized)
                    throw std::runtime_error("Cod is already present");
                main_header.cod = Cod(buf);
                break;

            case EJP2MarkerType::QCD:
                if (main_header.qcd.is_initialized)
                    throw std::runtime_error("Qcd is already present");
                main_header.qcd = Qcd(buf);
                break;

            case EJP2MarkerType::SOD:
                tile_parts[tile_parts.size() - 1].bit_stream = buf;
                break;

            case EJP2MarkerType::EOC:
                goto FIN;

            default:
                break;
        }

        index += len;
    }

    FIN:
}

auto Codestream::write() -> std::vector<uint8_t> {
    std::vector<uint8_t> res;

    push_vec(res, u16_to_bytes(EJP2MarkerType::SOC));

    main_header.siz.write(res);
    main_header.cod.write(res);
    main_header.qcd.write(res);

    for (TilePart tile_part : tile_parts) {
        tile_part.sot.write(res);
        res.insert(res.end(), tile_part.bit_stream.begin(), tile_part.bit_stream.end());
    }

    return res;
}

} // namespace kimp::jpeg2000::jp2
