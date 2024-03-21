#ifndef INCLUDE_BOX_H_
#define INCLUDE_BOX_H_

#include <cstdint>
#include <vector>
#include <fstream>

class box {
 friend class jp2;
 friend class superbox;
 public:
    box(std::ifstream& image_stream);

 private:
    uint32_t lbox_{};
    uint32_t tbox_{};
    uint64_t xlbox_{};
    std::vector<uint8_t> dbox_;
};

#endif
