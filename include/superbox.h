#ifndef INCLUDE_SUPERBOX_H_
#define INCLUDE_SUPERBOX_H_

#include <box.h>
#include <unordered_map>

class superbox {
 friend class jp2;
 public:
    superbox(std::ifstream& image_stream);
 private:
    uint32_t lbox_{};
    uint32_t tbox_{};
    uint64_t xlbox_{};
    std::vector<box> boxes_;

    std::unordered_map<uint32_t, std::vector<size_t>> box_position_;
};

#endif
