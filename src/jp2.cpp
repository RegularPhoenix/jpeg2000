#include <jp2.h>
#include <bit>
#include <algorithm>

uint32_t get_block_type(std::ifstream& image_stream) {
    auto current_position{image_stream.tellg()};
    image_stream.seekg(4, std::ios::cur);
    
    uint32_t block_type;
    image_stream.read(reinterpret_cast<char*>(&block_type), sizeof(block_type));
    block_type = std::byteswap(block_type);

    image_stream.seekg(current_position, std::ios::beg);
    return block_type;
}

jp2::jp2(const std::filesystem::path& image_path) {
    if(!std::filesystem::exists(image_path))
        throw std::runtime_error("Wrong image path");

    if(!std::filesystem::is_regular_file(image_path))
        throw std::runtime_error("Wrong file");

    image_stream_.open(image_path, std::ios::binary);
    image_size_ = std::filesystem::file_size(image_path);

    while(image_stream_.tellg() < image_size_) {
        uint32_t box_type{get_block_type(image_stream_)};

        switch(box_type) {
            //jp2h
            case 0x6A703268: {
                superbox jp2h_box(image_stream_);
                image_boxes_.push_back(jp2h_box);
                break;
            }

            //res 
            case 0x72657320: {
                superbox resolution_box(image_stream_);
                image_boxes_.push_back(resolution_box);
                break;
            }

            //uinf 
            case 0x75696E66: {
                superbox uuid_info_box(image_stream_);
                image_boxes_.push_back(uuid_info_box);
                break;
            }

            default: {
                box some_box(image_stream_);
                image_boxes_.push_back(some_box);
                break;
            }
        }
        box_position_[box_type].push_back(image_boxes_.size() - 1);
    }

    process_signature_box();
    process_profile_box();
    process_jp2_header_box();
    process_image_header_box();
}

void jp2::process_signature_box() {
    if(!box_position_.count(0x6A502020))
        throw std::runtime_error("No signature box");
    
    if(!box_position_[0x6A502020].size() > 1)
        throw std::runtime_error("Too many signature boxes");
    
    box signature_box{std::get<box>(
        image_boxes_[box_position_[0x6A502020][0]])};
    
    if(number_from_bytes<uint32_t>(signature_box.dbox_) != 0x0D0A870A)
        throw std::runtime_error("Wrong JP2 signature");
}

void jp2::process_profile_box() {
    if(!box_position_.count(0x7072666C))
        throw std::runtime_error("No profile box");

    if(!box_position_[0x7072666C].size() > 1)
        throw std::runtime_error("Too many profile boxes");
    
    box profile_box{std::get<box>(image_boxes_[box_position_[0x7072666C][0]])};
    if(profile_box.dbox_.size() % 4 || profile_box.dbox_.size() / 4 < 2)
        throw std::runtime_error("Profile box wrong format");
    
    if(number_from_bytes<uint32_t>({
            profile_box.dbox_.begin(),
            profile_box.dbox_.begin() + 4}) != 0x6A703240)
        throw std::runtime_error("Profile box wrong format");
    
    bool supported{};
    for(size_t i{}; i < profile_box.dbox_.size(); i += 4) {
        supported |= number_from_bytes<uint32_t>({
            profile_box.dbox_.begin() + i,
            profile_box.dbox_.begin() + i + 4}) != 0x6A703240;
    }

    if(!supported)
        throw std::runtime_error("Profile box wrong format");
}

void jp2::process_jp2_header_box() {
    if(!box_position_.count(0x6A703268))
        throw std::runtime_error("No jp2 header box");
    
    if(!box_position_[0x6A703268].size() > 1)
        throw std::runtime_error("Too many jp2 header boxes");
}

void jp2::process_image_header_box() {
    superbox jp2_header_box{
        std::get<superbox>(image_boxes_[box_position_[0x6A703268][0]])};

    if(!jp2_header_box.box_position_.count(0x69686472))
        throw std::runtime_error("No image header box");
    
    if(!jp2_header_box.box_position_[0x69686472].size() > 1)
        throw std::runtime_error("Too many image header boxes");

    box image_header_box{
        jp2_header_box.boxes_[jp2_header_box.box_position_[0x69686472][0]]};

    //xlbox_ == 24? xlbox_ == 22?
    if(image_header_box.xlbox_ != 24)
        throw std::runtime_error("Wrong image header size");

    uint16_t version{
        number_from_bytes<uint16_t>({
            image_header_box.dbox_.begin(),
            image_header_box.dbox_.begin() + 2})};
    
    uint16_t number_of_components{
        number_from_bytes<uint16_t>({
            image_header_box.dbox_.begin() + 2,
            image_header_box.dbox_.begin() + 4})};

    uint32_t image_height{
    number_from_bytes<uint32_t>({
        image_header_box.dbox_.begin() + 4,
        image_header_box.dbox_.begin() + 8})};
    
    uint32_t image_width{
    number_from_bytes<uint32_t>({
        image_header_box.dbox_.begin() + 8,
        image_header_box.dbox_.begin() + 12})};

    int8_t bit_depth{static_cast<int8_t>(image_header_box.dbox_[12])};

    uint8_t compression{image_header_box.dbox_[13]};    
    if(compression != 7)
        throw std::runtime_error("Wrong compression type");
    
    uint8_t colourspace_state{image_header_box.dbox_[14]};
    if(colourspace_state != 0 && colourspace_state != 1)
        throw std::runtime_error("Wrong colourspace state value");

    uint8_t intellectual_property{image_header_box.dbox_[15]};
    if(intellectual_property != 0 && intellectual_property != 1)
        throw std::runtime_error("Wrong intellectual property value");  
}

void jp2::process_colour_specification_box() {
    superbox jp2_header_box{
        std::get<superbox>(image_boxes_[box_position_[0x6A703268][0]])};

    if(!jp2_header_box.box_position_.count(0x636F6C72))
        throw std::runtime_error("No colour specification header box");
    
    if(!jp2_header_box.box_position_[0x636F6C72].size() > 1)
        throw std::runtime_error("Too many colour specification header boxes");

    
}