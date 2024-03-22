#include <jpeg2000/color/rbg.hpp>

namespace kimp::jpeg2000::color {

auto operator<<(std::ostream& out, const TRGB& c) -> std::ostream& {
    return out << "{ "
        << ".R = " << c.R << " "
        << ".G = " << c.G << " "
        << ".B = " << c.B << " "
        << "}";
}

} // namespace kimp::jpeg2000::color
