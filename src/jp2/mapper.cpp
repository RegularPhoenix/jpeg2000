#include "jpeg2000/jp2/mapper.hpp"
#include "jpeg2000/jp2/box.hpp"
#include "jpeg2000/jp2/raw.hpp"
#include "jpeg2000/utils/logging.hpp"
#include "jpeg2000/utils/membuf.hpp"
#include <jpeg2000/jpeg2000.hpp>

#include <memory>
#include <optional>
#include <stdexcept>
#include <streambuf>

namespace kimp::jpeg2000::jp2 {

TJP2BoxesMapper::TJP2BoxesMapper()
    : utils::TWithLogging {this}
    {}

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
    return nullptr;
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
