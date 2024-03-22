#pragma once

#include <jpeg2000/utils/num.hpp>

#include <streambuf>

namespace kimp::jpeg2000::utils {

class TMembuf : public std::streambuf {
public:
    TMembuf(const ui8* data, ui64 dataLen);

private:
    int_type underflow();
    int_type uflow();
    int_type pbackfail(int_type ch);
    std::streamsize showmanyc();
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
    std::streampos seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

    // copy ctor and assignment not implemented;
    // copying not allowed
    TMembuf(const TMembuf &);
    TMembuf &operator= (const TMembuf &);

private:
    const ui8 * const Begin_;
    const ui8 * const End_;
    const ui8 * Current_;
};

} // namespace kimp::jpeg2000::utils
