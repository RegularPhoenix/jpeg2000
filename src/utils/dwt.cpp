#include <functional>
#include <iostream>
#include <jpeg2000/utils/dwt.hpp>
#include <vector>

namespace kimp::jpeg2000::utils {

auto downDiv(i32 a, i32 b) -> i32 {
    if (a >= 0) {
        return a / b;
    } else {
        if (a % b == 0) {
            return a / b;
        } else {
            return a / b - 1;
        }
    }
}

auto upDiv(i32 a, i32 b) -> i32 {
    if (a % b > 0) {
        return a / b + 1;
    } else {
        return a / b;
    }
}

auto Do53DWT(const std::vector<i32>& src) -> std::vector<i32> {
    auto x = [&] (i32 i) {
        if (i < 0) {
            return src.at(-i);
        } else if (i >= src.size()) {
            return src.at(2 * src.size() - i - 2);
        } else {
            return src.at(i);
        }
    };

    std::function<i32(i32)> y = [&] (i32 i) {
        if (i % 2 == 0) {
            return x(i) + downDiv(y(i - 1) + y(i + 1) + 2, 4);
        } else {
            return x(i) - downDiv(x(i - 1) + x(i + 1), 2);
        }
    };
    
    std::vector<i32> result;
    result.reserve(src.size());

    for (i32 i {0}; i < src.size(); i++) {
        result.push_back(y(i));
    }

    return result;
}

auto Transpone(const std::vector<std::vector<i32>>& src) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> result;

    for (ui64 i {0}; i < src.at(0).size(); i++) {
        std::vector<i32> newRow;
        for (ui64 j {0}; j < src.size(); j++) {
            newRow.push_back(src.at(j).at(i));
        }
        result.push_back(newRow);
    }

    return result;
}


auto BaseMatrixDWT(const std::vector<std::vector<i32>>& src) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> temp;

    for (ui64 i {0}; i < src.at(0).size(); i++) {
        std::vector<i32> column;
        column.reserve(src.size());

        for (const auto& row : src) {
            column.push_back(row.at(i));
        }

        temp.push_back(Do53DWT(column));
    }

    temp = Transpone(temp);

    std::vector<std::vector<i32>> result;
    for (const auto& row : temp) {
        result.push_back(Do53DWT(row));
    }
    return result;
}

auto Deinterleave(const std::vector<std::vector<i32>>& src) -> TA {
    ui64 v = src.size(), u = src.at(0).size();

    std::vector<std::vector<i32>> ll;
    for (ui64 vb {0}; vb < upDiv(v, 2); vb++) {
        std::vector<i32> row;
        for (ui64 ub {0}; ub < upDiv(u, 2); ub++) {
            row.push_back(src.at(vb * 2).at(ub * 2));
        }
        ll.push_back(row);
    }

    std::vector<std::vector<i32>> hl;
    for (ui64 vb {0}; vb < upDiv(v, 2); vb++) {
        std::vector<i32> row;
        for (ui64 ub {0}; ub < downDiv(u, 2); ub++) {
            row.push_back(src.at(vb * 2).at(ub * 2 + 1));
        }
        hl.push_back(row);
    }

    std::vector<std::vector<i32>> lh;
    for (ui64 vb {0}; vb < downDiv(v, 2); vb++) {
        std::vector<i32> row;
        for (ui64 ub {0}; ub < upDiv(u, 2); ub++) {
            row.push_back(src.at(vb * 2 + 1).at(2 * ub));
        }
        lh.push_back(row);
    }

    std::vector<std::vector<i32>> hh;
    for (ui64 vb {0}; vb < downDiv(v, 2); vb++) {
        std::vector<i32> row;
        for (ui64 ub {0}; ub < downDiv(u, 2); ub++) {
            row.push_back(src.at(vb * 2 + 1).at(ub * 2 + 1));
        }
        hh.push_back(row);
    }

    return TA { .LL = ll, .HL = hl, .LH = lh, .HH = hh};
}


auto Union(const TA& ta) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> result;

    for (ui64 i {0}; i < ta.LL.size(); i++) {
        std::vector<i32> row;
        for (auto e : ta.LL.at(i)) {
            row.push_back(e);
        }
        for (auto e : ta.HL.at(i)) {
            row.push_back(e);
        }
        result.push_back(row);
    }

    for (ui64 i {0}; i < ta.HH.size(); i++) {
        std::vector<i32> row;
        for (auto e : ta.LH.at(i)) {
            row.push_back(e);
        }
        for (auto e : ta.HH.at(i)) {
            row.push_back(e);
        }
        result.push_back(row);
    }

    return result;
}

auto Union(const std::vector<TA>& d) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> combined = d.back().LL;

    for (ui64 i {d.size() - 1}; i > 0; i--) {
        TA current = d.at(i);
        combined = Union(TA { .LL = combined, .HL = current.HL, .LH = current.LH, .HH = current.HH });
    }

    return combined;

}

auto Do53DWT(const std::vector<std::vector<i32>>& src, ui32 levels) -> std::vector<TA> {
    std::vector<TA> result;
    result.push_back(TA { .LL = src, .HL = {}, .LH = {}, .HH = {} });

    for (ui64 i {1}; i <= levels; i++) {
        result.push_back(Deinterleave(BaseMatrixDWT(result.at(i - 1).LL)));
    }

    return result;
}

auto Undo53DWT(const std::vector<i32>& src) -> std::vector<i32> {
    auto y = [&] (i32 i) {
        if (i < 0) {
            return src.at(-i);
        } else if (i >= src.size()) {
            return src.at(2 * src.size() - i - 2);
        } else {
            return src.at(i);
        }
    };

    std::function<i32(i32)> x = [&] (i32 i) {
        if (i % 2 == 0) {
            return y(i) - downDiv(y(i - 1) + y(i + 1) + 2, 4);
        } else {
            return y(i) + downDiv(x(i - 1) + x(i + 1), 2);
        }
    };

    std::vector<i32> result;
    result.reserve(src.size());

    for (i32 i {0}; i < src.size(); i++) {
        result.push_back(x(i));
    }

    return result;
}

auto Interleave(const TA& t) -> std::vector<std::vector<i32>> {
    ui64 u = t.HL.at(0).size() + t.LH.at(0).size();
    ui64 v = t.LL.size() + t.HH.size();

    std::vector<std::vector<i32>> result;
    for (ui64 i {0}; i < v; i++) {
        std::vector<i32> row;
        for (ui64 j {0}; j < u; j++) {
            row.push_back(0);
        }
        result.push_back(row);
    }

    for (ui64 vb {0}; vb < upDiv(v, 2); vb++) {
        for (ui64 ub {0}; ub < upDiv(u, 2); ub++) {
            result[2 * vb][2 * ub] = t.LL[vb][ub];
        }
    }

    for (ui64 vb {0}; vb < upDiv(v, 2); vb++) {
        for (ui64 ub {0}; ub < downDiv(u, 2); ub++) {
            result[2 * vb][2 * ub + 1] = t.HL[vb][ub];
        }
    }

    for (ui64 vb {0}; vb < downDiv(v, 2); vb++) {
        for (ui64 ub {0}; ub < upDiv(u, 2); ub++) {
            result[2 * vb + 1][2 * ub] = t.LH[vb][ub];
        }
    }

    for (ui64 vb {0}; vb < downDiv(v, 2); vb++) {
        for (ui64 ub {0}; ub < downDiv(u, 2); ub++) {
            result[2 * vb + 1][2 * ub + 1] = t.HH[vb][ub];
        }
    }

    return result;
}

auto BaseMatrixIDWT(const std::vector<std::vector<i32>>& src) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> temp;

    for (const auto& row : src) {
        temp.push_back(Undo53DWT(row));
    }

    std::vector<std::vector<i32>> result;

    for (ui64 i {0}; i < temp.at(0).size(); i++) {
        std::vector<i32> column;
        column.reserve(temp.size());

        for (const auto& r: temp) {
            column.push_back(r.at(i));
        }

        result.push_back(Undo53DWT(column));
    }

    return Transpone(result);

    /*for (ui64 i {0}; i < src.at(0).size(); i++) {
        std::vector<i32> column;
        column.reserve(src.size());

        for (const auto& row : src) {
            column.push_back(row.at(i));
        }

        temp.push_back(Do53DWT(column));
    }

    temp = Transpone(temp);

    std::vector<std::vector<i32>> result;
    for (const auto& row : temp) {
        result.push_back(Do53DWT(row));
    }
    return result;*/
}

auto Undo53DWT(const std::vector<TA>& src) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> ll = BaseMatrixIDWT(Interleave(src.back()));
    for (ui64 i {src.size() - 2}; i > 0; i--) {
        ll = BaseMatrixIDWT(Interleave(
            TA {.LL = ll, .HL = src[i].HL, .LH = src[i].LH, .HH = src[i].HH}
        ));
    }
    return ll;// BaseMatrixIDWT(Interleave(src.back()));
}

} // namespace kimp::jpeg2000::utils
