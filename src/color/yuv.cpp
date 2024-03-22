#include <jpeg2000/color/yuv.hpp>

namespace kimp::jpeg2000::color {

auto operator<<(std::ostream& out, const TYUV& c) -> std::ostream& {
    return out << "{ "
        << ".Y = " << c.Y << " "
        << ".U = " << c.U << " "
        << ".V = " << c.V << " "
        << "}";
}

} // namespace kimp::jpeg2000::color
