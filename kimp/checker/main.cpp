#include "jpeg2000/jp2/box.hpp"
#include "jpeg2000/jp2/mapper.hpp"
#include <fstream>

#include <iostream>
#include <jpeg2000/jpeg2000.hpp>
#include <memory>

#include <jpeg2000/color/rbg.hpp>
#include <jpeg2000/color/yuv.hpp>
#include <jpeg2000/image/image.hpp>
#include <vector>

#include <jpeg2000/jp2/coder.hpp>
#include <jpeg2000/utils/dwt.hpp>

using namespace kimp::jpeg2000;

auto main(int argc, char ** argv) -> int {
    image::TImage<color::TRGB> test(8, 8, color::TRGB {255, 255, 255});

    //jp2::TJP2Coder::NewBuilder()
    //    .SetRDepth(0)
    //    .SetGDepth(1)
    //    .SetBDepth(2)
    //    .Build()
    //    .Encode(test);    

    utils::Do53DWT({
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
        {3, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12},
        {4, 4, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12},
        {5, 5, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13},
        {6,6,6,6,7,7,8,9,10,10,11,12,13},
        {7,7,7,7,8,8,9,9,10,11,12,13,13},
        {8,8,8,8,8,9,10,10,11,12,12,13,14},
        {9,9,9,9,9,10,10,11,12,12,13,14,15},
        {10,10,10,10,10,11,11,12,12,13,14,14,15},
        {11,11,11,11,11,12,12,13,13,14,14,15,16},
        {12,12,12,12,12,13,13,13,14,15,15,16,16},
        {13,13,13,13,13,13,14,14,15,15,16,17,17},
        {14,14,14,14,14,14,15,15,16,16,17,17,18},
        {15,15,15,15,15,15,16,16,17,17,18,18,19},
        {16,16,16,16,16,16,17,17,17,18,18,19,20}
    }, 2);

    return 0;
}
