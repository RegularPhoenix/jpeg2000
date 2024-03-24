#pragma once

#include <jpeg2000/utils/num.hpp>

#include <ostream>
#include <vector>

namespace kimp::jpeg2000::image {

template <typename Pixel>
class TImage {
public:
    TImage(ui64 width, ui64 height, const Pixel& filler = 0)
        : Data_{}
        , Height_{height}
        , Width_{width}
    {
        Data_.reserve(Height_);
        for (ui64 i {0}; i < Height_; i++) {
            std::vector<Pixel> row;
            row.reserve(Width_);

            for (ui64 j {0}; j < Width_; j++) {
                row.push_back(Pixel {filler});
            }

            Data_.push_back(row);
        }
    }

    TImage(const std::vector<std::vector<Pixel>>& source)
        : Data_{source}
        , Height_{source.size()}
        , Width_{source.size() ? source.at(0).size() : 0}
        {}

    auto GetWidth() const -> ui64 {
        return Width_; 
    }

    auto GetHeight() const -> ui64 {
        return Height_;
    }

    auto GetPixel(ui64 wIndex, ui64 hIndex) const -> Pixel {
        return Data_.at(hIndex).at(wIndex);
    }

    friend auto operator<<(std::ostream& out, const TImage<Pixel>& img) -> std::ostream& {
        for (ui64 i {0}; i < img.GetHeight(); i++) {
            out << "[";
            for (ui64 j {0}; j < img.GetWidth(); j++) {
                out << " " << img.GetPixel(j, i);
            }
            out << " ]" << std::endl << std::flush;
        }
        return out;
    }

private:
    std::vector<std::vector<Pixel>> Data_;

    const ui64 Height_;
    const ui64 Width_;
};

} // namespace kimp::jpeg2000::image
