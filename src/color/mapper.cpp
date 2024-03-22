#include <jpeg2000/color/mapper.hpp>

namespace kimp::jpeg2000::color {

auto MapToYUV(const TRGB& source) -> TYUV {
    i32 tmp = 0;
    tmp += source.R;
    tmp += source.G;
    tmp += source.G;
    tmp += source.B;

    return TYUV {
        .Y = tmp / 4,
        .U = source.B - source.G,
        .V = source.R - source.G
    };
}

auto MapToRGB(const TYUV& source) -> TRGB {
    i32 temp = 0;
    temp += source.U;
    temp += source.V;
    i32 g = source.Y - temp / 4;

    return TRGB {
        .R = source.V + g,
        .G = g,
        .B = source.U + g
    };
};

} // namespace kimp::jpeg2000::color
