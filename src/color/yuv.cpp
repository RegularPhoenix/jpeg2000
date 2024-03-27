#include <iomanip>
#include <jpeg2000/color/yuv.hpp>

namespace kimp::jpeg2000::color {

auto operator<<(std::ostream& out, const TYUV& c) -> std::ostream& {
    return out << "{ "
        << std::setw(4) << std::setfill(' ') << c.Y << " "
        << std::setw(4) << std::setfill(' ') << c.U << " "
        <<  std::setw(4) << std::setfill(' ')<< c.V << " "
        << "}";
}

} // namespace kimp::jpeg2000::color
