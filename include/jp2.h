#ifndef INCLUDE_JP2_H_
#define INCLUDE_JP2_H_

#include <filesystem>
#include <fstream>
#include <variant>
#include <unordered_map>
#include <box.h>
#include <superbox.h>

template<class T>
T number_from_bytes(const std::vector<uint8_t>& data) {
    if(data.size() > 10 || (data.size() & (data.size() - 1)))
        return 0;

    T result{};
    for(auto &x : data) {
        result <<= (sizeof(uint8_t) * 8);
        result |= x;
    }
    return result;
}


class jp2 {
 public:
    jp2(const std::filesystem::path& image_path);

 private:
    std::ifstream image_stream_;
    std::size_t image_size_;
    std::unordered_map<uint32_t, std::vector<size_t>> box_position_;
    std::vector<std::variant<box, superbox>> image_boxes_;

    void process_signature_box();
    void process_profile_box();
    void process_jp2_header_box();
    void process_image_header_box();
    void process_colour_specification_box();
};

#endif
