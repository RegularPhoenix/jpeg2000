#pragma once

#include <jpeg2000/utils/num.hpp>

#include <vector>

namespace kimp::jpeg2000::utils {

auto Do53DWT(const std::vector<std::vector<i32>>& src, ui32 levels) -> void;

} // namespace kimp::jpeg2000::utils
