#include "jpeg2000/utils/logging.hpp"
#include <jpeg2000/jp2/raw.hpp>

namespace kimp::jpeg2000::jp2 {

TRawJP2Box::TRawJP2Box()
    : utils::TWithLogging(this)
    , BoxContentSize_{0}
    , BoxType_{0}
    , BoxContent_{nullptr}
    {}

TRawJP2Box::TRawJP2Box(EJP2BoxType type, const std::vector<ui8>& content)
    : utils::TWithLogging(this)
    , BoxContentSize_{content.size()}
    , BoxType_{static_cast<ui32>(type)}
    , BoxContent_{new ui8 [content.size()]}
{
    for (ui64 i {0}; i < BoxContentSize_; i++) {
        BoxContent_[i] = content.at(i);
    }
}

auto TRawJP2Box::GetContentSize() const -> ui64 {
    return BoxContentSize_;
}

auto TRawJP2Box::GetContentType() const -> ui32 {
    return BoxType_;
}

auto TRawJP2Box::GetContent() const -> const ui8 * {
    return BoxContent_;
}

auto TRawJP2Box::Write(std::ostream & out) const -> void {
    Logger_->info("Going to write raw box to stream");
    WriteBoxPreamble(out);
    WriteBoxContent(out);
    Logger_->info("Box was succesfully written");
}

auto TRawJP2Box::WriteBoxPreamble(std::ostream& out) const -> void {
    auto writeDWord = [&out] (ui32 dword) {
        char buf;
        for (int i {0}; i < 4; i++) {
            buf = static_cast<char>((dword >> (8 * (3 - i))) & 0x000000ff);
            out.write(&buf, 1);
        }
    };

    if (BoxContentSize_ > (0xffffffff - 1)) {
        Logger_->info("Box size is too long to store in LBox ({}), gonna use XLBox", BoxContentSize_);
        writeDWord(1);        
        writeDWord(BoxType_);
        writeDWord(static_cast<ui32>((BoxContentSize_ + 4 + 4 + 8) >> 32));
        writeDWord(static_cast<ui32>((BoxContentSize_ + 4 + 4 + 8) & 0x00000000ffffffff));
    } else {
        Logger_->info("Box preamble will consist from LBox and TBox only");
        writeDWord(static_cast<ui32>(BoxContentSize_ + 4 + 4));
        writeDWord(BoxType_);
    }
}

auto TRawJP2Box::WriteBoxContent(std::ostream& out) const -> void {
    Logger_->info("Gonna to write content block of {} bytes", BoxContentSize_);
    out.write(reinterpret_cast<char*>(BoxContent_), BoxContentSize_);
}

auto TRawJP2Box::Read(std::istream & in) -> void {
    Logger_->info("Reading of new raw JP2 block started");
    ReadBoxPreamble(in);
    ReadBoxContent(in);
    Logger_->info("Box was successfully read");
    in.peek();
}

auto TRawJP2Box::ReadBoxPreamble(std::istream& in) -> void {
    auto readDWord = [&in] () {
        ui32 dword = 0; ui8 buf;
        for (int i {0}; i < 4; i++) {
            in.read(reinterpret_cast<char*>(&buf), 1);
            dword = (dword << 8) + buf;
        }
        return dword;
    };

    ui32 readSize = readDWord();
    ui32 readType = readDWord();

    if (readSize == 1) {
        ui64 xlReadSize = (static_cast<ui64>(readDWord()) << 32) + readDWord();
        Logger_->info("LBox is 1, gonna use XLBox ({} bytes)", xlReadSize);
        BoxContentSize_ = xlReadSize - 4 - 4 - 8;
    } else if (readSize == 0) {
        Logger_->info("Content will take all last file part");
        BoxContentSize_ = 0;
    } else {
        Logger_->info("Got size of current box (LBox): {} bytes", readSize);
        BoxContentSize_ = readSize - 4 - 4;
    }

    Logger_->info("Current box type is {:#x} ({:.4})", readType, reinterpret_cast<char*>(&readType));
    BoxType_ = readType;
}

auto TRawJP2Box::ReadBoxContent(std::istream& in) -> void {
    if (BoxContent_) {
        Logger_->warn("Overriding old box content");
        delete [] BoxContent_;
        BoxContent_ = nullptr;
    }

    if (BoxContentSize_ > 0) {
        Logger_->info("Gonna read {} content bytes", BoxContentSize_);

        BoxContent_ = new ui8 [BoxContentSize_];
        in.read(reinterpret_cast<char*>(BoxContent_), BoxContentSize_);
    } else {
        auto currentFilePos = in.tellg();
        in.seekg(0, std::ios_base::end);
        auto lastPos = in.tellg();

        in.seekg(currentFilePos, std::ios_base::beg);
        BoxContentSize_ = lastPos - currentFilePos;
        ReadBoxContent(in);
    }
}

TRawJP2Box::~TRawJP2Box() {
    if (BoxContent_) {
        delete [] BoxContent_;
        BoxContent_ = nullptr;
    }
}

} // namespace kimp::jpeg2000::jp2
