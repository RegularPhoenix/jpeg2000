#pragma once

#include <jpeg2000/utils/num.hpp>

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

} // namespace kimp::jpeg2000::jp2
