#include "codestream.h"
#include "helper.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

bool Codestream::read_segment(const std::vector<uint8_t>& data, size_t& byte_ind, uint16_t marker) {
    switch (static_cast<Marker>(marker)) {
        case Marker::SOT:
            context = Context::TilePart;
            SOT::read_to_codestream(*this, data, byte_ind);
            break;

        case Marker::SOD:
            context = Context::InBitStream;
            // TODO: Read tile-part data
            break;

        case Marker::SIZ:
            SIZ::read_to_codestream(*this, data, byte_ind);
            break;

        case Marker::EOC:
            if (context == Context::MainHeader) {
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

Codestream::Codestream(const std::vector<uint8_t>& data) {
    size_t byte_ind{};
    if (static_cast<Marker>(bytes_to_u16(data[byte_ind], data[byte_ind + 1])) != Marker::SOC) {
        throw std::runtime_error("Expected SOC marker!");
    }
    byte_ind += 2;

    uint16_t cur_marker = bytes_to_u16(data[byte_ind], data[byte_ind + 1]);
    byte_ind += 2;
    while (!read_segment(data, byte_ind, cur_marker)) {
        cur_marker = bytes_to_u16(data[byte_ind], data[byte_ind + 1]);
        byte_ind += 2;
    }

    // TODO: Ensure all required segments are present
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
