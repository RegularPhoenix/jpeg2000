#include <superbox.h>
#include <bit>

superbox::superbox(std::ifstream& image_stream) {
    image_stream.read(reinterpret_cast<char*>(&lbox_), sizeof(lbox_));
    image_stream.read(reinterpret_cast<char*>(&tbox_), sizeof(tbox_));
    lbox_ = std::byteswap(lbox_);
    tbox_ = std::byteswap(tbox_);

    size_t box_definition_size{sizeof(lbox_) + sizeof(tbox_)};
    
    if(lbox_ >= 2 && lbox_ <= 7)
        throw std::runtime_error("Wrong lbox value");

    if (lbox_ == 1) {
        image_stream.read(reinterpret_cast<char*>(&xlbox_), sizeof(xlbox_));
        xlbox_ = std::byteswap(xlbox_);

        box_definition_size += sizeof(xlbox_);
    }
    else if (lbox_ == 0) {
        auto current_position = image_stream.tellg();
        image_stream.seekg(0, std::ios::end);
        
        xlbox_ = image_stream.tellg() - current_position;
        image_stream.clear();
        image_stream.seekg(current_position);

        xlbox_ += sizeof(lbox_);
        xlbox_ += sizeof(tbox_);
        box_definition_size = xlbox_;
    }
    else {
        xlbox_ = lbox_;
    }

    size_t temp{box_definition_size};
    while(temp < xlbox_) {
        box current_box(image_stream);
        box_position_[current_box.tbox_].push_back(boxes_.size());
        boxes_.push_back(current_box);
        temp += current_box.xlbox_;
    }
}