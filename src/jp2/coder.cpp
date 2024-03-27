#include <functional>
#include <iomanip>
#include <jpeg2000/color/mapper.hpp>
#include "jpeg2000/color/rbg.hpp"
#include "jpeg2000/color/yuv.hpp"

#include "jpeg2000/image/image.hpp"
#include "jpeg2000/utils/dwt.hpp"
#include <jpeg2000/jp2/coder.hpp>

#include <cmath>
#include <iostream>
#include <vector>

namespace kimp::jpeg2000::jp2 {

template <typename T>
auto GetComponent(const TImage<T>& src, const std::function<i32(T)>& componentGetter) -> std::vector<std::vector<i32>> {
    std::vector<std::vector<i32>> result;
    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<i32> row;
        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(componentGetter(src.GetPixel(j, i)));
        }
        result.push_back(row);
    }
    return result;
}

auto operator<<(std::ostream& out, const std::vector<std::vector<i32>>& data) -> std::ostream& {
    for (const auto& row : data) {
        for (const auto p : row) {
            out << std::setw(4) << std::setfill(' ') << p << " ";
        }
        out << std::endl;
    }
    return out;
}

auto ApplyDCLevelShifitng(const std::vector<std::vector<i32>>& data, ui8 depth) -> std::vector<std::vector<i32>> {
    i32 delta = (depth & 0x80) ? 0 : std::pow(2, depth);
    std::vector<std::vector<i32>> result;
    for (const auto& row : data) {
        std::vector<i32> tr;
        for (const auto v : row) {
            tr.push_back(v - delta);
        }
        result.push_back(tr);
    }
    return result;
}

auto TJP2Coder::Encode(const TImage<TRGB>& src) -> void {
    std::cout << "Красный канал" << std::endl;
    auto red = GetComponent<TRGB>(src, [] (TRGB r) { return r.R; } );
    std::cout << red << std::endl;

    std::cout << "Красный канал после DC level shifting с depth = 0" << std::endl;
    auto dcRed = ApplyDCLevelShifitng(red, 0);
    std::cout << dcRed << std::endl;

    std::cout << "Зеленый канал" << std::endl;
    auto green = GetComponent<TRGB>(src, [] (TRGB r) { return r.G; } );
    std::cout << green << std::endl;

    std::cout << "Зеленый канал после DC level shifting с depth = 1" << std::endl;
    auto dcGreen = ApplyDCLevelShifitng(green, 1);
    std::cout << dcGreen << std::endl;

    std::cout << "Синий канал" << std::endl;
    auto blue = GetComponent<TRGB>(src, [] (TRGB r) { return r.B; } );
    std::cout << blue << std::endl;

    std::cout << "Синий канал после DC level shifting с depth = 2" << std::endl;
    auto dcBlue = ApplyDCLevelShifitng(blue, 2);
    std::cout << dcBlue << std::endl;

    std::vector<std::vector<TRGB>> dcSrc;
    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<TRGB> row;
        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(
                TRGB {
                    .R = dcRed[j][i],
                    .G = dcGreen[j][i],
                    .B = dcBlue[j][i]
                }
            );
        }
        dcSrc.push_back(row);
    }

    std::cout << "Изображение после dc level shifting (depth is 0, 1, 2)" << std::endl;
    std::cout << TImage<TRGB>(dcSrc) << std::endl;

    auto yuvSrc = ApplyReversibleMultipleComponentTramsform(TImage<TRGB>(dcSrc));
    std::cout << "Изображение после Multiple Component Transformation (RGB -> YUV)" << std::endl;
    std::cout << yuvSrc << std::endl;

    auto y = GetComponent<TYUV>(yuvSrc, [] (TYUV p) { return p.Y; });
    std::cout << "Y канал изображения" << std::endl;
    std::cout << y << std::endl;

    auto yDwt1 = utils::Do53DWT(y, 1);
    std::cout << "Y канал изображения после DWT (1 уровень)" << std::endl;
    std::cout << utils::Union(yDwt1) << std::endl;

    auto yDwt2 = utils::Do53DWT(y, 2);
    std::cout << "Y канал изображения после DWT (2 уровня)" << std::endl;
    std::cout << utils::Union(yDwt2) << std::endl;

    std::cout << "Обратное Вейвлет преобразование Y канала (1 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(yDwt1) << std::endl;
    std::cout << "Обратное Вейвлет преобразование Y канала (2 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(yDwt2) << std::endl;

    auto u = GetComponent<TYUV>(yuvSrc, [] (TYUV p) { return p.U; });
    std::cout << "U канал изображения" << std::endl;
    std::cout << u << std::endl;

    auto uDwt1 = utils::Do53DWT(u, 1);
    std::cout << "U канал изображения после DWT (1 уровень)" << std::endl;
    std::cout << utils::Union(uDwt1) << std::endl;

    auto uDwt2 = utils::Do53DWT(u, 2);
    std::cout << "U канал изображения после DWT (2 уровня)" << std::endl;
    std::cout << utils::Union(uDwt2) << std::endl;

    std::cout << "Обратное Вейвлет преобразование U канала (1 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(uDwt1) << std::endl;
    std::cout << "Обратное Вейвлет преобразование U канала (2 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(uDwt2) << std::endl;

    auto v = GetComponent<TYUV>(yuvSrc, [] (TYUV p) { return p.V; });
    std::cout << "V канал изображения" << std::endl;
    std::cout << v << std::endl;

    auto vDwt1 = utils::Do53DWT(v, 1);
    std::cout << "Y канал изображения после DWT (1 уровень)" << std::endl;
    std::cout << utils::Union(vDwt1) << std::endl;

    auto vDwt2 = utils::Do53DWT(v, 2);
    std::cout << "Y канал изображения после DWT (2 уровня)" << std::endl;
    std::cout << utils::Union(vDwt2) << std::endl;

    std::cout << "Обратное Вейвлет преобразование V канала (1 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(vDwt1) << std::endl;
    std::cout << "Обратное Вейвлет преобразование V канала (2 уровень)" << std::endl;
    std::cout << utils::Undo53DWT(vDwt2) << std::endl;
    
    TImage<TRGB> backRgb = ApplyInverseReversibleMultipleComponentTramsform(yuvSrc);
    std::cout << "Inverse multiple component transformation (YUV -> RGB)" << std::endl;
    std::cout << backRgb << std::endl;

    std::cout << "Inverse DC level shifting" << std::endl;
    std::cout << ApplyInverseDCLevelShifting(backRgb) << std::endl;
}

auto TJP2Coder::ApplyDCLevelShifting(const TImage<TRGB>& src) const -> TImage<TRGB> {
    std::vector<std::vector<TRGB>> result;
    result.reserve(src.GetHeight());

    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<TRGB> row;
        row.reserve(src.GetWidth());

        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(ApplyDCLevelShifting(src.GetPixel(j, i)));
        }

        result.push_back(row);
    }

    return TImage<TRGB> {result};
}

auto TJP2Coder::ApplyDCLevelShifting(const TRGB& src) const -> TRGB {
    ui8 deltaR = (RDepth_ & 0x80) ? 0 : std::pow(2, RDepth_);
    ui8 deltaG = (GDepth_ & 0x80) ? 0 : std::pow(2, GDepth_);
    ui8 deltaB = (BDepth_ & 0x80) ? 0 : std::pow(2, BDepth_);
    return TRGB {
        .R = static_cast<ui8>(src.R - deltaR),
        .G = static_cast<ui8>(src.G - deltaG),
        .B = static_cast<ui8>(src.B - deltaB)
    };
};

auto TJP2Coder::ApplyReversibleMultipleComponentTramsform(const TImage<TRGB>& src) const -> TImage<TYUV> {
    std::vector<std::vector<TYUV>> result;
    result.reserve(src.GetHeight());

    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<TYUV> row;
        row.reserve(src.GetWidth());

        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(MapToYUV(src.GetPixel(j, i)));
        }

        result.push_back(row);
    }

    return TImage<TYUV> {result};
}

auto TJP2Coder::ApplyInverseDCLevelShifting(const TImage<TRGB>& src) const -> TImage<TRGB> {
    std::vector<std::vector<TRGB>> result;
    result.reserve(src.GetHeight());

    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<TRGB> row;
        row.reserve(src.GetWidth());

        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(ApplyInverseDCLevelShifting(src.GetPixel(j, i)));
        }

        result.push_back(row);
    }

    return TImage<TRGB> {result};
}

auto TJP2Coder::ApplyInverseDCLevelShifting(const TRGB& src) const -> TRGB {
    ui8 deltaR = (RDepth_ & 0x80) ? 0 : std::pow(2, RDepth_);
    ui8 deltaG = (GDepth_ & 0x80) ? 0 : std::pow(2, GDepth_);
    ui8 deltaB = (BDepth_ & 0x80) ? 0 : std::pow(2, BDepth_);
    return TRGB {
        .R = static_cast<ui8>(src.R + deltaR),
        .G = static_cast<ui8>(src.G + deltaG),
        .B = static_cast<ui8>(src.B + deltaB)
    };
};

auto TJP2Coder::ApplyInverseReversibleMultipleComponentTramsform(const TImage<TYUV>& src) const -> TImage<TRGB> {
    std::vector<std::vector<TRGB>> result;
    result.reserve(src.GetHeight());

    for (ui64 i {0}; i < src.GetHeight(); i++) {
        std::vector<TRGB> row;
        row.reserve(src.GetWidth());

        for (ui64 j {0}; j < src.GetWidth(); j++) {
            row.push_back(MapToRGB(src.GetPixel(j, i)));
        }

        result.push_back(row);
    }

    return TImage<TRGB> {result};
}

auto TJP2Coder::TBuilder::Build() const -> TJP2Coder {
    return TJP2Coder { rd, gd, bd };
}

auto TJP2Coder::TBuilder::SetRDepth(ui8 rDepth) -> TJP2Coder::TBuilder& {
    this->rd = rDepth;
    return (*this);
}

auto TJP2Coder::TBuilder::SetGDepth(ui8 gDepth) -> TJP2Coder::TBuilder& {
    this->gd = gDepth;
    return (*this);
}

auto TJP2Coder::TBuilder::SetBDepth(ui8 bDepth) -> TJP2Coder::TBuilder& {
    this->bd = bDepth;
    return (*this);
}

TJP2Coder::TBuilder::TBuilder() {}

auto TJP2Coder::NewBuilder() -> TJP2Coder::TBuilder {
    return TJP2Coder::TBuilder {};
}

TJP2Coder::TJP2Coder(ui8 rDepth, ui8 gDepth, ui8 bDepth)
    : RDepth_{rDepth}
    , GDepth_{gDepth}
    , BDepth_{bDepth}
    {}

} // namespace kimp::jpeg2000::jp2
