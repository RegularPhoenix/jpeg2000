#pragma once

#include <jpeg2000/utils/num.hpp>

#include <ostream>

namespace kimp::jpeg2000::color {

struct TYUV {
public:
    const i32 Y;
    const i32 U;
    const i32 V;

    friend auto operator<<(std::ostream& out, const TYUV& c) -> std::ostream&;
};

} // namespace kimp::jpeg2000::color
