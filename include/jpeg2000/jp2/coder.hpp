#pragma once

#include <jpeg2000/color/rbg.hpp>
#include <jpeg2000/color/yuv.hpp>
#include <jpeg2000/image/image.hpp>
#include <jpeg2000/utils/num.hpp>

namespace kimp::jpeg2000::jp2 {

using namespace image;
using namespace color;

class TJP2Coder;

class TJP2Coder {
public:

    auto Encode(const TImage<TRGB>& src) -> void;

public:
    class TBuilder {
    public:
        auto Build() const -> TJP2Coder;

        auto SetRDepth(ui8 rDepth) -> TBuilder&;
        auto SetGDepth(ui8 gDepth) -> TBuilder&;
        auto SetBDepth(ui8 bDepth) -> TBuilder&;

    private:
        friend class TJP2Coder;
        TBuilder();

    private:
        ui8 rd = 0, gd = 0, bd = 0;
    };

public:
    static auto NewBuilder() -> TBuilder;

private:
    friend class TBuilder;
    TJP2Coder(ui8 rDepth, ui8 gDepth, ui8 bDepth);

private:
    auto ApplyDCLevelShifting(const TImage<TRGB>& src) const -> TImage<TRGB>;
    auto ApplyDCLevelShifting(const TRGB& src) const -> TRGB;

    auto ApplyReversibleMultipleComponentTramsform(const TImage<TRGB>& src) const -> TImage<TYUV>;

    auto ApplyInverseDCLevelShifting(const TImage<TRGB>& src) const -> TImage<TRGB>;
    auto ApplyInverseDCLevelShifting(const TRGB& src) const -> TRGB;
    
    auto ApplyInverseReversibleMultipleComponentTramsform(const TImage<TYUV>& src) const -> TImage<TRGB>;

private:
    const ui8 RDepth_;
    const ui8 GDepth_;
    const ui8 BDepth_;
};

} // namespace kimp::jpeg2000::jp2
