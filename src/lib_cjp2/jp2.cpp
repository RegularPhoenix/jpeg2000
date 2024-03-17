#include "jp2.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

#include "codestream.h"
#include "helper.h"

Box Box::read(std::ifstream &reader) {
    Box box = Box();
    box.reader = &reader;

    box.lbox = read_u32(reader);
    if (box.lbox < 8 && box.lbox != 0 && box.lbox != 1) {
        throw std::runtime_error("A box has incorrect length!");
    }

    box.tbox = read_u32(reader);

    if (box.lbox == 1) {
        box.xlbox = read_u64(reader);
        if (box.xlbox < 16) {
            throw std::runtime_error("A box has incorrect length!");
        }

        //	read_exact(box.dbox, reader, box.xlbox - 16);
        //} else {
        //	read_exact(box.dbox, reader, box.lbox == 0 ? reader.right : box.lbox - 8);
    }

    box.dbox_start = reader.tellg();

    reader.seekg(box.dbox_start + (box.lbox == 1 ? box.xlbox - 16 : box.lbox - 8));

    return box;
}

// template<typename T>
// T Box::unbox() {
//	return *reinterpret_cast<T*>(&dbox);
// }

template <typename T>
T Box::unbox() {
    reader->seekg(dbox_start);

    std::vector<uint8_t> buf;

    if (lbox == 1) {
        read_exact(buf, *reader, xlbox);
    } else {
        read_exact(buf, *reader, lbox == 0 ? reader->right : lbox - 8);
    }

    return *reinterpret_cast<T *>(&buf);
}

JP2 JP2::read(std::ifstream &reader) {
    JP2 result = JP2();

    Box signature_box = Box::read(reader);

    if (signature_box.tbox != 0x6A501A1A || signature_box.unbox<uint32_t>() != 0x0D0A870A) {
        throw std::runtime_error("File signature does not match the JP2 signature!");
    }

    while (reader.eof()) {
        Box current_box = Box::read(reader);

        switch (current_box.tbox) {
            case 0x7072666C:  // 'prfl'
                result.profile_box = current_box.unbox<Profile>();
                break;

            case 0x6A703263:  // 'jp2c'
                reader.seekg(current_box.dbox_start);
                result.codestream = Codestream::read(reader);
                break;

            case 0x6A703268:  // 'jp2h'
                reader.seekg(current_box.dbox_start);
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

        if (current_box.lbox == 0) {
            break;
        }
    }

    return result;
}
