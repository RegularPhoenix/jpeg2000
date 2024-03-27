#include "jpeg2000/jp2/mapper.hpp"
#include "jpeg2000/jp2/box.hpp"
#include "jpeg2000/jp2/raw.hpp"
#include "jpeg2000/utils/logging.hpp"
#include "jpeg2000/utils/membuf.hpp"
#include "jpeg2000/utils/num.hpp"
#include <fstream>
#include <jpeg2000/jpeg2000.hpp>

#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <vector>

namespace kimp::jpeg2000::jp2 {

TJP2BoxesMapper::TJP2BoxesMapper()
    : utils::TWithLogging {this}
    {}

auto TJP2BoxesMapper::MarshalBox(const std::shared_ptr<TJP2Box>& box) const -> TRawJP2Box {
    if (box->GetType() == EJP2BoxType::SIGNATURE_BOX) {
        return MarshalSignatureBox(std::dynamic_pointer_cast<TSignatureJP2Box>(box));
    } else if (box->GetType() == EJP2BoxType::FILE_TYPE_BOX) {
        return MarshalFileTypeBox(std::dynamic_pointer_cast<TFileTypeJP2Box>(box));
    } else if (box->GetType() == EJP2BoxType::JP2H_CONTAINER) {
        return MarshalHeaderContainerBox(std::dynamic_pointer_cast<THeaderContainerJP2Box>(box));
    } else if (box->GetType() == EJP2BoxType::JP2H_IMAGE_HEADER) {
        return MarshalImageHeaderBox(std::dynamic_pointer_cast<TImageHeaderJP2Box>(box));
    } else if (box->GetType() == EJP2BoxType::JP2H_COLOR_SPECIFICATION) {
        return MarshalColorSpecificationBox(std::dynamic_pointer_cast<TColorSpecificationJP2Box>(box));
    } else if (box->GetType() == EJP2BoxType::CODE_STREAM_BOX) {
        return MarshalCodeStreamBox(std::dynamic_pointer_cast<TCodeStreamJP2Box>(box));
    }

    throw std::invalid_argument("Unable to marshal box");
}

auto TJP2BoxesMapper::MarshalSignatureBox(std::shared_ptr<TSignatureJP2Box> box) const -> TRawJP2Box {
    std::vector<ui8> content;
    Write(content, box->GetSignature());
    return TRawJP2Box(EJP2BoxType::SIGNATURE_BOX, content);
}

auto TJP2BoxesMapper::MarshalFileTypeBox(std::shared_ptr<TFileTypeJP2Box> box) const -> TRawJP2Box {
    std::vector<ui8> content;
    Write(content, box->GetBrand());
    Write(content, box->GetMinorVersion());
    for (auto c : box->GetCompatibilities()) {
        Write(content, c);
    }
    return TRawJP2Box(EJP2BoxType::FILE_TYPE_BOX, content);
}

auto TJP2BoxesMapper::MarshalHeaderContainerBox(std::shared_ptr<THeaderContainerJP2Box> box) const -> TRawJP2Box {
    std::vector<ui8> content;
    for (const auto& child : box->GetChildren()) {
        auto childBox = MarshalBox(child);
        
        if (childBox.GetContentSize() > (0xffffffff - 1)) {
            Write(content, static_cast<ui32>(1));
            Write(content, childBox.GetContentType());
            Write(content, static_cast<ui32>((childBox.GetContentSize() + 16) >> 32));
            Write(content, static_cast<ui32>((childBox.GetContentSize() + 16) & 0x00000000ffffffff));
        } else {
            Write(content, static_cast<ui32>(childBox.GetContentSize() + 8));
            Write(content, childBox.GetContentType());
        }

        for (ui64 i {0}; i < childBox.GetContentSize(); i++) {
            content.push_back(childBox.GetContent()[i]);
        }
    }
    return TRawJP2Box(EJP2BoxType::JP2H_CONTAINER, content);
}

auto TJP2BoxesMapper::MarshalImageHeaderBox(std::shared_ptr<TImageHeaderJP2Box> box) const -> TRawJP2Box {
    std::vector<ui8> content;
    Write(content, box->GetHeight());
    Write(content, box->GetWidth());
    Write(content, box->GetNumberOfComponents());
    Write(content, box->GetBitsPerComponent());
    Write(content, box->GetCompressionType());
    Write(content, static_cast<ui8>(box->IsColorspaceUnknown()));
    Write(content, box->IsIntellectualProperty());
    return TRawJP2Box(EJP2BoxType::JP2H_IMAGE_HEADER, content);
}

auto TJP2BoxesMapper::MarshalColorSpecificationBox(std::shared_ptr<TColorSpecificationJP2Box> box) const -> TRawJP2Box {
    std::vector<ui8> content;
    
    Write(content, box->GetMeth());
    Write(content, box->GetPrec());
    Write(content, box->GetApprox());
    
    if (box->GetEnumeratedColorSpace()) {
        Write(content, box->GetEnumeratedColorSpace().value());
    }

    if (box->GetICCProfile()) {
        std::vector<ui8> icc = box->GetICCProfile().value();
        for (auto b : icc) {
            content.push_back(b);
        }
    }
    
    return TRawJP2Box(EJP2BoxType::JP2H_COLOR_SPECIFICATION, content);
}

auto TJP2BoxesMapper::MarshalCodeStreamBox(std::shared_ptr<TCodeStreamJP2Box> box) const -> TRawJP2Box {
    return TRawJP2Box(EJP2BoxType::CODE_STREAM_BOX, box->GetCode());
}

auto TJP2BoxesMapper::UnmarshalRawBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TJP2Box> {
    if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::SIGNATURE_BOX)) {
        return UnmarshalSignatureBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::FILE_TYPE_BOX)) {
        return UnmarshalFileTypeBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::JP2H_CONTAINER)) {
        return UnmarshalHeaderCotainerBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::JP2H_IMAGE_HEADER)) {
        return UnmarshalImageHeaderBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::XML_BOX)) {
        return UnmarshalXmlBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::JP2H_COLOR_SPECIFICATION)) {
        return UnmarshalColorSpecificationBox(rawBox);
    } else if (rawBox.GetContentType() == static_cast<ui32>(EJP2BoxType::CODE_STREAM_BOX)) {
        return UnmarshalCodeStreamBox(rawBox);
    }

    throw std::invalid_argument("Unable to unmarshal your raw box");
}

auto TJP2BoxesMapper::UnmarshalSignatureBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TSignatureJP2Box> {
    if (rawBox.GetContentSize() != 4) throw std::invalid_argument("Cannot unmarshal signature box, invalid size");
    ui32 signatureValue = Read<ui32>(rawBox.GetContent());

    Logger_->info("Mapping to signature box (signature is {:#x})", signatureValue);
    return std::make_shared<TSignatureJP2Box>(signatureValue);
}

auto TJP2BoxesMapper::UnmarshalFileTypeBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TFileTypeJP2Box> {
    if (rawBox.GetContentSize() % 4) throw std::invalid_argument("Cannot unmarshal file type box, invalid size");

    int compatibilitiesNumber = rawBox.GetContentSize() / 4 - 2;
    
    std::vector<ui32> compatibilities (compatibilitiesNumber);
    for (int i {0}; i < compatibilitiesNumber; i++) {
        compatibilities[i] = Read<ui32>(rawBox.GetContent() + 8 + i * 4);
    }

    ui32 brand = Read<ui32>(rawBox.GetContent()), minorVersion = Read<ui32>(rawBox.GetContent() + 4);

    Logger_->info("Mapping to file type box (brand is {:.4}, version is {}, has {} compatibilitie(s))", reinterpret_cast<char*>(&brand), minorVersion, compatibilitiesNumber);
    return std::make_shared<TFileTypeJP2Box>(brand, minorVersion, compatibilities);
}

auto TJP2BoxesMapper::UnmarshalHeaderCotainerBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<THeaderContainerJP2Box> {
    std::vector<std::shared_ptr<TJP2Box>> children;

    utils::TMembuf membuf(rawBox.GetContent(), rawBox.GetContentSize());
    std::istream in(&membuf);

    while (!in.eof()) {
        TRawJP2Box tempRaw; tempRaw.Read(in);
        children.push_back(UnmarshalRawBox(tempRaw));
    }

    Logger_->info("Decoding header");
    return std::make_shared<THeaderContainerJP2Box>(children);
}

auto TJP2BoxesMapper::UnmarshalImageHeaderBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TImageHeaderJP2Box> {
    if (rawBox.GetContentSize() != 14) throw std::invalid_argument("Cannot unmarshal image header box, invalid size");

    ui32 height = Read<ui32>(rawBox.GetContent()), width = Read<ui32>(rawBox.GetContent() + 4);
    ui16 nc = Read<ui16>(rawBox.GetContent() + 8);

    ui8 bpc = Read<ui8>(rawBox.GetContent() + 10)
        , c = Read<ui8>(rawBox.GetContent() + 11)
        , unkC = Read<ui8>(rawBox.GetContent() + 12)
        , ipr = Read<ui8>(rawBox.GetContent() + 13);

    Logger_->info("Mapping to image header box ({}x{} with {} components)", height, width, nc);
    return std::make_shared<TImageHeaderJP2Box>(height, width, nc, bpc, c, unkC, ipr);
}

auto TJP2BoxesMapper::UnmarshalXmlBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TXmlJP2Box> {
    pugi::xml_document doc;
    
    auto parseResult = doc.load_buffer(rawBox.GetContent(), rawBox.GetContentSize());
    if (!parseResult) {
        throw std::invalid_argument(fmt::format("Unable to parse xml content: {}", parseResult.description()));
    }

    Logger_->info("Mapping to xml box");
    return std::make_shared<TXmlJP2Box>(std::move(doc));
}

auto TJP2BoxesMapper::UnmarshalColorSpecificationBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TColorSpecificationJP2Box> {
    ui8 meth = Read<ui8>(rawBox.GetContent() + 0)
        , prec = Read<ui8>(rawBox.GetContent() + 1)
        , approx = Read<ui8>(rawBox.GetContent() + 2);

    std::optional<ui32> enumCS = meth == 1 
        ? std::make_optional(Read<ui32>(rawBox.GetContent() + 3))
        : std::nullopt;

    std::optional<std::vector<ui8>> icc = std::nullopt;
    if (meth == 2) {
        icc = std::make_optional(std::vector<ui8> (rawBox.GetContentSize() - 3));
        icc->assign(rawBox.GetContent() + 3, rawBox.GetContent() + rawBox.GetContentSize());
    }

    Logger_->info("Mapping to color specification, meth is {}", meth);
    return std::make_shared<TColorSpecificationJP2Box>(meth, prec, approx, enumCS, icc);
}

auto TJP2BoxesMapper::UnmarshalCodeStreamBox(const TRawJP2Box& rawBox) const -> std::shared_ptr<TCodeStreamJP2Box> {
    std::vector<ui8> code(rawBox.GetContentSize());
    code.assign(rawBox.GetContent(), rawBox.GetContent() + rawBox.GetContentSize());
    
    Logger_->info("Mapping to code stream of {} bytes", rawBox.GetContentSize());
    return std::make_shared<TCodeStreamJP2Box>(code);
}

} // namespace kimp::jpeg2000::jp2
