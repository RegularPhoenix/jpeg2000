#pragma once

#include <jpeg2000/utils/num.hpp>

#include <vector>

namespace kimp::jpeg2000::utils {

struct TA {
public:
    std::vector<std::vector<i32>> LL, HL;
    std::vector<std::vector<i32>> LH, HH;
};

auto Union(const TA& ta) -> std::vector<std::vector<i32>>;

auto Union(const std::vector<TA>& d) -> std::vector<std::vector<i32>>;

auto Do53DWT(const std::vector<i32>& src) -> std::vector<i32>;

auto Do53DWT(const std::vector<std::vector<i32>>& src, ui32 levels) -> std::vector<TA>;

auto Undo53DWT(const std::vector<i32>& src) -> std::vector<i32>;

auto Undo53DWT(const std::vector<TA>& src) -> std::vector<std::vector<i32>>;

} // namespace kimp::jpeg2000::utils
