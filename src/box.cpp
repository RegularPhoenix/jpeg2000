#include <box.h>
#include <bit>

box::box(std::ifstream& image_stream) {
    image_stream.read(reinterpret_cast<char*>(&lbox_), sizeof(lbox_));
    image_stream.read(reinterpret_cast<char*>(&tbox_), sizeof(tbox_));
    lbox_ = std::byteswap(lbox_);
    tbox_ = std::byteswap(tbox_);

    if(lbox_ >= 2 && lbox_ <= 7)
        throw std::runtime_error("Wrong lbox value");

    if (lbox_ == 1) {
        image_stream.read(reinterpret_cast<char*>(&xlbox_), sizeof(xlbox_));
        xlbox_ = std::byteswap(xlbox_);

        dbox_.assign(xlbox_ - sizeof(lbox_) - sizeof(tbox_) - sizeof(xlbox_), 0);
    }
    else if (lbox_ == 0) {
        auto current_position = image_stream.tellg();
        image_stream.seekg(0, std::ios::end);
        
        xlbox_ = image_stream.tellg() - current_position;
        image_stream.clear();
        image_stream.seekg(current_position);

        dbox_.assign(xlbox_ - sizeof(lbox_) - sizeof(tbox_) - sizeof(xlbox_), 0);
    }
    else {
        xlbox_ = lbox_;
        dbox_.assign(xlbox_ - sizeof(lbox_) - sizeof(tbox_), 0);
    }

    for (auto&x : dbox_)
        image_stream.read(reinterpret_cast<char*>(&x), 1);
}