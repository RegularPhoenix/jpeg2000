#include <cassert>
#include <jpeg2000/utils/membuf.hpp>

namespace kimp::jpeg2000::utils {

TMembuf::TMembuf(const ui8* data, ui64 dataLen)
    : std::streambuf {}
    , Begin_{data}
    , End_{data + dataLen}
    , Current_{data}
    {}

TMembuf::int_type TMembuf::underflow() {
    if (Current_ == End_) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*Current_);
}

TMembuf::int_type TMembuf::uflow() {
    if (Current_ == End_) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*Current_++);
}

TMembuf::int_type TMembuf::pbackfail(int_type ch) {
    if (Current_ == Begin_ || (ch != traits_type::eof() && ch != Current_[-1])) {
        return traits_type::eof();
    }
    return traits_type::to_int_type(*--Current_);
}

std::streamsize TMembuf::showmanyc() {
    assert(std::less_equal<const uint8_t *>()(Current_, End_));
    return End_ - Current_;
}


std::streampos TMembuf::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which ) {
    if (way == std::ios_base::beg) {
        Current_ = Begin_ + off;
    } else if (way == std::ios_base::cur) {
        Current_ += off; 
    } else if (way == std::ios_base::end) {
        Current_ = End_ + off;
    }

    if (Current_ < Begin_ || Current_ > End_) {
        return -1;
    }
    return Current_ - Begin_;
}

std::streampos TMembuf::seekpos(std::streampos sp, std::ios_base::openmode which) {
    Current_ = Begin_ + sp;
    if (Current_ < Begin_ || Current_ > End_) {
        return -1;
    }
    return Current_ - Begin_;
}

} //namespace kimp::jpeg2000::utils