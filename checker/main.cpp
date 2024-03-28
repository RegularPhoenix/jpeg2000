#include "jpeg2000/jp2/box.hpp"
#include "jpeg2000/jp2/codestream.hpp"
#include "jpeg2000/jp2/mapper.hpp"
#include "jpeg2000/jp2/raw.hpp"
#include <fstream>

#include <ios>
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
    std::vector<std::vector<color::TRGB>> example;

    ui64 imageSize = 8;
    i32 step = 256 / imageSize;

    for (ui64 i {1}; i <= imageSize; i++) {
        std::vector<color::TRGB> row;
        for (ui64 j {1}; j <= imageSize; j++) {
            color::TRGB p {
                .R = static_cast<i32>(step * i - 1),
                .G = static_cast<i32>(step * i / 2 + step * j / 2 - 1),
                .B = static_cast<i32>(step * j - 1)
            };
            row.push_back(p);
        }
        example.push_back(row);
    }

    std::cout << "Исходное изображение" << std::endl;
    std::cout << image::TImage<color::TRGB>(example) << std::endl;

    std::ofstream fout("check.pnm", std::ios_base::out | std::ios_base::trunc);
    fout << "P3" << std::endl;
    fout << imageSize << " " << imageSize << std::endl;
    fout << 255 << std::endl;

    for (const auto& row : example) {
        for (const auto& c : row) {
            fout << c.R << " " << c.G << " " << c.B << std::endl;
        }
    }
    fout.close();

    jp2::TJP2Coder::NewBuilder()
    .SetRDepth(0)
    .SetGDepth(1)
    .SetBDepth(2)
    .Build()
    .Encode(image::TImage<color::TRGB>(example));

    std::ifstream fin(argv[1]);
    std::vector<std::shared_ptr<jp2::TJP2Box>> boxes;

    std::cout << "Чтение боксов из " << argv[1] << std::endl << std::endl;

    jp2::TJP2BoxesMapper mapper;

    while (!fin.eof()) {
        jp2::TRawJP2Box rawBox;
        rawBox.Read(fin);
        boxes.push_back(mapper.UnmarshalRawBox(rawBox));
    }

std::cout << std::endl;
    std::cout << "Боксы из файла" << std::endl;
    for (const auto & b : boxes) {
        std::cout << b->ToString() << std::endl;
    }

    auto cs = std::dynamic_pointer_cast<jp2::TCodeStreamJP2Box>(boxes.back());
    jp2::Codestream codestream(cs->GetCode());

    
    std::cout << "Прочитан codestream" << std::endl << std::endl;

    std::cout << "Заголовок SIZ:" << std::endl;
    auto siz = codestream.main_header.siz;
    std::cout << "LSIZ " << siz.rsiz << std::endl;
    std::cout << "XSIZ " << siz.xsiz << std::endl;
    std::cout << "YSIZ " << siz.ysiz << std::endl;
    std::cout << "XOSIZ " << siz.xosiz << std::endl;
    std::cout << "YOSIZ " << siz.yosiz << std::endl;
    std::cout << "XTOSIZ " << siz.xtosiz << std::endl;
    std::cout << "YTOSIZ " << siz.ytosiz << std::endl;
    std::cout << "CSIZ " << siz.csiz << std::endl << std::endl;

    std::cout << "Заголовок COD" << std::endl;
    auto cod = codestream.main_header.cod;
    std::cout << "LCOD " << cod.lcod << std::endl;
    std::cout << "SCOD " << static_cast<ui32>(cod.scod) << std::endl << std::endl;

    std::cout << "Заголовок QCD" << std::endl;
    auto qcd = codestream.main_header.qcd;
    std::cout << "LQCD " << qcd.lqcd << std::endl;
    std::cout << "SQCD " << static_cast<ui32>(qcd.sqcd) << std::endl << std::endl;

    for (const auto& tp : codestream.tile_parts) {
        std::cout << "Found tile part" << std::endl;
        std::cout << "SOT section of tile part" << std::endl;
        std::cout << "LSOT " << tp.sot.lsot << std::endl; 
        std::cout << "PSOT " << tp.sot.psot << std::endl;
        std::cout << "ISOT " << tp.sot.isot << std::endl;
        std::cout << "Tile part has " << tp.bit_stream.size() << " bytes inside" << std::endl;
        std::cout << "First bytes of content ";
        for (int i = 0; i < 10; i++) std::cout << "0x"<< std::hex << (int)tp.bit_stream.at(i) << " ";
        std::cout << std::endl << std::endl;
    }

    std::cout << std::endl;
    std::ofstream fj("new.jp2");

    std::cout << "Запись проитанных боксов в new.jp2" << std::endl << std::endl;

    std::cout << std::hex;
    for (const auto& b : boxes) {
        mapper.MarshalBox(b).Write(fj);
    }

    return 0;
}
