#pragma once

#include "jpeg2000/jp2/box.hpp"
#include "jpeg2000/jp2/raw.hpp"
#include "jpeg2000/utils/logging.hpp"
#include <jpeg2000/jpeg2000.hpp>

#include <memory>

namespace kimp::jpeg2000::jp2 {

class TJP2BoxesMapper : utils::TWithLogging {
public:
    TJP2BoxesMapper();

    auto UnmarshalRawBox(const TRawJP2Box&) const -> std::shared_ptr<TJP2Box>;

private:
    auto UnmarshalSignatureBox(const TRawJP2Box&) const -> std::shared_ptr<TSignatureJP2Box>;
    auto UnmarshalFileTypeBox(const TRawJP2Box&) const -> std::shared_ptr<TFileTypeJP2Box>;
    auto UnmarshalHeaderCotainerBox(const TRawJP2Box&) const -> std::shared_ptr<THeaderContainerJP2Box>;
    auto UnmarshalImageHeaderBox(const TRawJP2Box&) const -> std::shared_ptr<TImageHeaderJP2Box>;
    auto UnmarshalXmlBox(const TRawJP2Box&) const -> std::shared_ptr<TXmlJP2Box>;
    auto UnmarshalColorSpecificationBox(const TRawJP2Box&) const -> std::shared_ptr<TColorSpecificationJP2Box>;
    auto UnmarshalCodeStreamBox(const TRawJP2Box&) const -> std::shared_ptr<TCodeStreamJP2Box>;

private:
    template <typename T>
    auto Read(const ui8* buf) const -> T {
        T res = 0;
        for (int i = 0; i < sizeof (T); i++) {
            res = (res << 8) + buf[i];
        }
        return res;
    }
};

} // namespace kimp::jpeg2000::jp2
