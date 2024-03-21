#include <iostream>
#include <jpeg2000/utils/dwt.hpp>
#include <vector>

namespace kimp::jpeg2000::utils {

struct TA_ {
    std::vector<std::vector<i32>> LL, HL;
    std::vector<std::vector<i32>> LH, HH;
};

auto _1D_EXTD(const std::vector<i32>& a, ui64 i0, ui64 i1) -> std::vector<i32> {
    std::vector<i32> result;

    if (i0 % 2 == 0) {
        if (a.size() == 1) {
            result.push_back(a[0]);
            result.push_back(a[0]);
        } else if (result.size() == 2) {
            result.push_back(a[1]);
            result.push_back(a[0]);
        } else {
            result.push_back(a[1]);
            result.push_back(a[2]);
        }
    } else {
        if (a.size() == 1) {
            result.push_back(a[0]);
        } else {
            result.push_back(a[1]);
        }
    }

    for (i32 v : a) {
        result.push_back(v);
    }

    if (i1 % 2 == 0) {
        if (a.size() > 1) {
            result.push_back(a[a.size() - 2]);
        } else {
            result.push_back(a[a.size() - 1]);
        }
    } else {
        if (a.size() > 2) {
            result.push_back(a[a.size() - 2]);
            result.push_back(a[a.size() - 3]);
        } else if (a.size() == 2) {
            result.push_back(a[a.size() - 2]);
            result.push_back(a[a.size() - 1]);
        } else {
            result.push_back(a.back());
            result.push_back(a.back());
        }
    }

    return result;
}

auto _1D_FILTR(const std::vector<i32>& y, ui64 i0, ui64 i1) -> std::vector<i32> {
    std::vector<i32> x (i1 - i0 + 1);

    for (ui64 n {i0 / 2}; n < i1 / 2 + 1; n++) {
        x[2*n] = y[2*n] - (y[2*n - 1] + y[2*n + 1] + 2) / 4;
    }

    for (ui64 n {i0 / 2}; n < i1 / 2; n++) {
        x[2*n + 1] = y[2*n + 1] - (x[2*n] + x[2*n + 2]) / 2;
    }

    return x;
}

auto _1D_SD(const std::vector<i32>& a, ui64 i0, ui64 i1) -> std::vector<i32> {
    auto xExt = _1D_EXTD(a, i0, i1);
    auto yExt = _1D_FILTR(xExt, i0, i1);
    return yExt;
}

auto _VER_SD(const std::vector<std::vector<i32>>& aLL, ui64 u0, ui64 u1, ui64 v0, ui64 v1) -> std::vector<std::vector<i32>> {
    ui64 u = u0, i0 = v0, i1 = v1;

    std::vector<std::vector<i32>> result;
    result.reserve(u1 - u0);

    while (u < u1) {
        std::vector<i32> column;
        column.reserve(v1 - v0);
        for (ui64 h {v0}; h < v1; h++) {
            column.push_back(aLL[h][u]);
        }
        result.push_back(_1D_SD(column, i0, i1));
        u = u + 1;
    }
    
    return result;
}

auto _HOR_SD(const std::vector<std::vector<i32>>& aLL, ui64 u0, ui64 u1, ui64 v0, ui64 v1) -> std::vector<std::vector<i32>> {
    ui64 v = v0, i0 = u0, i1 = u1;

    std::vector<std::vector<i32>> result;
    result.reserve(v1 - v0);

    while (!(v >= v1)) {
        std::vector<i32> row;
        row.reserve(u1 - u0);
        for (ui64 w {u0}; w < u1; w++) {
            row.push_back(aLL[w][v]);
        }
        result.push_back(_1D_SD(row, i0, i1));
        v = v + 1;
    }

    return result;
}

auto _2D_DEINTERLEAVE(const std::vector<std::vector<i32>>& a, ui64 u0, ui64 u1, ui64 v0, ui64 v1) -> TA_ {

    return TA_{{},{},{},{}};
}

auto _2D_SD(const std::vector<std::vector<i32>>& aLL, ui64 u0, ui64 u1, ui64 v0, ui64 v1) -> TA_ {
    auto a = _VER_SD(aLL, u0, u1, v0, v1);
    auto b = _HOR_SD(a, u0, u1, v0, v1);
    return TA_ {{}, {}, {}, {}};
}

auto Do53DWT(const std::vector<std::vector<i32>>& src, ui32 levels) -> void {
    std::vector<TA_> a;
    a.reserve(levels + 1);

    a.push_back(TA_ { .LL = src, .HL = {}, .LH = {}, .HH = {} });

    for (ui32 i {1}; i <= levels; i++) {
        a.push_back(_2D_SD(a[i - 1].LL, 0, a[i - 1].LL.at(0).size(), 0, a[i - 1].LL.size()));
        break;
    }
}

} // namespace kimp::jpeg2000::utils
