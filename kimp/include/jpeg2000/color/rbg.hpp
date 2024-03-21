#pragma once

#include <jpeg2000/utils/num.hpp>

#include <ostream>

namespace kimp::jpeg2000::color {

struct TRGB {
public:
    const i32 R;
    const i32 G;
    const i32 B;

    friend auto operator<<(std::ostream& out, const TRGB& c) -> std::ostream&;
};

} // namespace kimp::jpeg200::color

