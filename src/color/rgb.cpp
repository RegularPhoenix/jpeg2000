#include <iomanip>
#include <jpeg2000/color/rbg.hpp>

namespace kimp::jpeg2000::color {

auto operator<<(std::ostream& out, const TRGB& c) -> std::ostream& {
    return out <<  "{ "
        <<std::setw(3) << std::setfill(' ') << c.R << " "
        <<std::setw(3) << std::setfill(' ') << c.G << " "
        <<std::setw(3) << std::setfill(' ') << c.B << " "
        << "}";
}

} // namespace kimp::jpeg2000::color
