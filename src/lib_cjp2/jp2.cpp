#include "jp2.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "codestream.h"
#include "helper.h"

void Box::read(std::ifstream &reader) {
    reader_ = &reader;

    lbox_ = read_u32(reader);
    if (lbox_ < 8 && lbox_ != 0 && lbox_ != 1) {
        throw std::runtime_error("A box has incorrect length!");
    }

    tbox_ = read_u32(reader);

    if (lbox_ == 1) {
        xlbox_ = read_u64(reader);
        if (xlbox_ < 16) {
            throw std::runtime_error("A box has incorrect length!");
        }

        //	read_exact(box.dbox, reader, box.xlbox - 16);
        //} else {
        //	read_exact(box.dbox, reader, box.lbox == 0 ? reader.right : box.lbox - 8);
    }

    dbox_.assign((lbox_ == 1 ? xlbox_ - 16 : lbox_ - 8), 0);
    dbox_start_ = reader.tellg();

    reader.seekg(dbox_start_ + dbox_.size());
}

// template<typename T>
// T Box::unbox() {
//	return *reinterpret_cast<T*>(&dbox);
// }

template <typename T>
T Box::unbox() {
    reader->seekg(dbox_start_);
    if (lbox == 1) {
        read_exact(dbox_, *reader, xlbox);
    } else {
        read_exact(dbox_, *reader, lbox == 0 ? reader->right : lbox - 8);
    }

    return *reinterpret_cast<T *>(&buf);
}

JP2 JP2::read(std::ifstream &reader) {
    JP2 result = JP2();

    Box signature_box;
    signature_box.read(reader);

    if (signature_box.tbox_ != 0x6A501A1A || signature_box.unbox<uint32_t>() != 0x0D0A870A) {
        throw std::runtime_error("File signature does not match the JP2 signature!");
    }

    while (reader.eof()) {
        Box current_box;
        current_box.read(reader);

        switch (current_box.tbox_) {
            case 0x7072666C:  // 'prfl'
                result.profile_box = current_box.unbox<Profile>();
                break;

            case 0x6A703263:  // 'jp2c'
                reader.seekg(current_box.dbox_start_);
                result.codestream = Codestream(current_box.dbox_);
                break;

            case 0x6A703268:  // 'jp2h'
                reader.seekg(current_box.dbox_start_);
                break;

            case 0x69686472:  // 'ihdr'
                result.image_header = current_box.unbox<ImageHeader>();
                // TODO: Validate values
                break;

            case 0x636F6C72:  // 'colr'
                result.colour_specification = ColourSpecification();
                result.colour_specification.meth = read_u8(reader);
                result.colour_specification.prec = read_u8(reader);
                result.colour_specification.approx = read_u8(reader);
                if (result.colour_specification.meth == 1) {
                    result.colour_specification.meth = read_u32(reader);
                }
                // TODO: Validate values
                break;

                // TODO: Optional boxes

            default:
                // Skip unknown boxes
                break;
        }

        if (current_box.lbox_ == 0) {
            break;
        }
    }

    return result;
}
