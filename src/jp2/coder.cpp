#include <jpeg2000/color/mapper.hpp>
#include "jpeg2000/color/yuv.hpp"

#include "jpeg2000/image/image.hpp"
#include <jpeg2000/jp2/coder.hpp>

#include <cmath>
#include <iostream>

namespace kimp::jpeg2000::jp2 {

auto TJP2Coder::Encode(const TImage<TRGB>& src) -> void {
    std::cout << "Goind go encode an image:" << std::endl << src << std::endl;
    
    std::cout << "Apply DC level shifting with " << TRGB { RDepth_, GDepth_, BDepth_ } << " depths:" << std::endl;
    auto dcShifted = ApplyDCLevelShifting(src);
    std::cout << dcShifted << std::endl; 
    // std::cout << "Inverse DC level shifting check:" << std::endl << ApplyInverseDCLevelShifting(dcShifted) << std::endl;

    std::cout << "Apply reversible multiply component transformation:" << std::endl;
    auto yuvImage = ApplyReversibleMultipleComponentTramsform(dcShifted);
    std::cout << yuvImage << std::endl;
    //std::cout << "Inverse multiply component transformation check:" << std::endl << ApplyInverseReversibleMultipleComponentTramsform(yuvImage) << std::endl;

    
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
