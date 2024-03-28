#pragma once

#include "jpeg2000/jp2/box.hpp"
#include <jpeg2000/utils/logging.hpp>
#include <jpeg2000/utils/num.hpp>

#include <iostream>
#include <vector>

namespace kimp::jpeg2000::jp2 {

class TRawJP2Box : utils::TWithLogging {
public:
    TRawJP2Box();
    TRawJP2Box(EJP2BoxType type, const std::vector<ui8>& content);

    auto GetContentSize() const -> ui64;
    auto GetContentType() const -> ui32;
    auto GetContent() const -> const ui8*;

    auto Write(std::ostream&) const -> void;
    auto Read(std::istream&) -> void;

    ~TRawJP2Box();

private:
    auto WriteBoxPreamble(std::ostream&) const -> void;
    auto WriteBoxContent(std::ostream&) const -> void;

    auto ReadBoxPreamble(std::istream&) -> void;
    auto ReadBoxContent(std::istream&) -> void;

private:
    ui64 BoxContentSize_;
    ui32 BoxType_;
    ui8* BoxContent_;
};

} // namespace kimp::jpeg2000::jp2
