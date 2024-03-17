#include "codestream.h"
#include "helper.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

bool read_segment(Codestream &codestream, std::ifstream &reader, uint16_t marker) {
    switch (static_cast<Marker>(marker)) {
        case Marker::SOT:
            codestream.context = Context::TilePart;
            SOT::read_to_codestream(codestream, reader);
            break;

        case Marker::SOD:
            codestream.context = Context::InBitStream;
            // TODO: Read tile-part data
            break;

        case Marker::SIZ:
            SIZ::read_to_codestream(codestream, reader);
            break;

        case Marker::EOC:
            if (codestream.context == Context::MainHeader) {
                throw std::runtime_error("Unexpected EOC! (Codestream does not contain any tile-parts)");
            } else {
                return true;
            }
            break;

        default:
            // Skip instead of throwing?
            throw std::runtime_error("Unknown marker!");
            break;
    }

    return false;
}

Codestream Codestream::read(std::ifstream &reader) {
    Codestream codestream = Codestream();

    if (static_cast<Marker>(read_u16(reader)) != Marker::SOC) {
        throw std::runtime_error("Expected SOC marker!");
    }

    uint16_t cur_marker = read_u16(reader);
    while (!read_segment(codestream, reader, cur_marker)) {
        cur_marker = read_u16(reader);
    }

    // TODO: Ensure all required segments are present

    return codestream;
}

void Codestream::ensure_context(Context valid_context) {
    if (context != valid_context) {
        throw std::runtime_error("Invalid context");
    }
}

template <typename T>
bool TilePartHeader::is_marker_present() {
    for (int i = 0; i < segments.size(); i++) {
        if (reinterpret_cast<T>(segments[i]) != nullptr)
            return true;
    }

    return false;
}

template <typename T>
bool MainHeader::is_marker_present() {
    for (int i = 0; i < segments.size(); i++) {
        if (dynamic_cast<T*>(&segments[i]) != nullptr)
            return true;
    }

    return false;
}

template <typename T>
T MainHeader::get_marker_segment() {
    for (int i = 0; i < segments.size(); i++) {
        T marker = reinterpret_cast<T>(segments[i]);
        if (marker != nullptr)
            return marker;
    }

    throw std::runtime_error("Could not get segment");
}
