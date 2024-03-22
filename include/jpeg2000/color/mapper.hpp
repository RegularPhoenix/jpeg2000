#pragma once

#include <jpeg2000/color/rbg.hpp>
#include <jpeg2000/color/yuv.hpp>

namespace kimp::jpeg2000::color {

auto MapToYUV(const TRGB& source) -> TYUV;

auto MapToRGB(const TYUV& source) -> TRGB;

} // namespace kimp::jpeg2000::color
