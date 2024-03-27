#include "jpeg2000/jp2/box.hpp"
#include "fmt/format.h"
#include "jpeg2000/utils/num.hpp"
#include <jpeg2000/jpeg2000.hpp>
#include <string>
#include <vector>

namespace kimp::jpeg2000::jp2 {

TJP2Box::TJP2Box(EJP2BoxType boxType)
    : BoxType_{boxType}
    {}

auto TJP2Box::GetType() const -> EJP2BoxType {
    return BoxType_;
}

TJP2Box::~TJP2Box() {}


TSignatureJP2Box::TSignatureJP2Box(ui32 signature)
    : TJP2Box {EJP2BoxType::SIGNATURE_BOX}
    , Signature_{signature}
    {}

auto TSignatureJP2Box::GetSignature() const -> ui32 {
    return Signature_;
}

auto TSignatureJP2Box::IsSignatureValid() const -> bool {
    return Signature_ == VALID_SIGNATURE;
}

std::string TSignatureJP2Box::ToString() const {
    return fmt::format("Signatue box: {:x}", Signature_);
}

TSignatureJP2Box::~TSignatureJP2Box() {}


TFileTypeJP2Box::TFileTypeJP2Box(
    ui32 brand
    , ui32 minorVersion
    , const std::vector<ui32>& compatibilities
) : TJP2Box {EJP2BoxType::FILE_TYPE_BOX}
    , Brand_{brand}
    , MinorVersion_{minorVersion}
    , Compatibilities_{compatibilities}
    {}

auto TFileTypeJP2Box::GetBrand() const -> ui32 {
    return Brand_;
}

auto TFileTypeJP2Box::GetMinorVersion() const -> ui32 {
    return MinorVersion_;
}

auto TFileTypeJP2Box::GetCompatibilities() const -> const std::vector<ui32>& {
    return Compatibilities_;
}

auto TFileTypeJP2Box::IsJPEG2000() const -> bool {
    return Brand_ == JPEG2000_BRAND;
}

std::string TFileTypeJP2Box::ToString() const {
    return fmt::format("File type box: brand = {}, minor version = {}, compatibilities = {}", Brand_, MinorVersion_, fmt::join(Compatibilities_, ", "));
}

TFileTypeJP2Box::~TFileTypeJP2Box() {}


THeaderContainerJP2Box::THeaderContainerJP2Box(
    const std::vector<std::shared_ptr<TJP2Box>>& children
) : TJP2Box {EJP2BoxType::JP2H_CONTAINER}
    , Children_{children}
    {}

auto THeaderContainerJP2Box::GetChildren() const -> const std::vector<std::shared_ptr<TJP2Box>>& {
    return Children_;
}

std::string THeaderContainerJP2Box::ToString() const {
    std::string res = "";
    for (const auto& c : Children_) res += c->ToString() + "\n";
    return res;
}

THeaderContainerJP2Box::~THeaderContainerJP2Box() {}


TImageHeaderJP2Box::TImageHeaderJP2Box(
    ui32 height
    , ui32 width
    , ui16 nc
    , ui8 bpc
    , ui8 c
    , ui8 unkC
    , ui8 ipr
) : TJP2Box {EJP2BoxType::JP2H_IMAGE_HEADER}
    , Height_{height}
    , Width_{width}
    , NumberOfComponents_{nc}
    , BitsPerComponent_{bpc}
    , CompessionType_{c}
    , IsColorspaceUnknown_{unkC}
    , IsIntellectualProperty_{ipr}
    {}

auto TImageHeaderJP2Box::GetHeight() const -> ui32 {
    return Height_;
}

auto TImageHeaderJP2Box::GetWidth() const -> ui32 {
    return Width_;
}

auto TImageHeaderJP2Box::GetNumberOfComponents() const -> ui16 {
    return NumberOfComponents_;
}

auto TImageHeaderJP2Box::GetBitsPerComponent() const -> ui8 {
    return BitsPerComponent_;
}

auto TImageHeaderJP2Box::GetCompressionType() const -> ui8 {
    return CompessionType_;
}

auto TImageHeaderJP2Box::IsColorspaceUnknown() const -> bool {
    return IsColorspaceUnknown_ == 1;
}

auto TImageHeaderJP2Box::IsIntellectualProperty() const -> bool {
    return IsIntellectualProperty_ == 1;
}

auto TImageHeaderJP2Box::IsJPEG2000() const -> bool {
    return CompessionType_ == JPEG2000_COMPRESSION_TYPE;
}

std::string TImageHeaderJP2Box::ToString() const {
    return fmt::format("Image header box: size is {}x{}, has {} components, compression is {}", Width_, Height_, NumberOfComponents_, CompessionType_);
}

TImageHeaderJP2Box::~TImageHeaderJP2Box() {}


TXmlJP2Box::TXmlJP2Box(pugi::xml_document&& doc)
    : TJP2Box{EJP2BoxType::XML_BOX}
    , XmlDoc_(std::move(doc))
    {}

auto TXmlJP2Box::GetXmlDoc() const -> const pugi::xml_document& {
    return XmlDoc_;
}

std::string TXmlJP2Box::ToString() const {
    return "XML box with some content";
}

TXmlJP2Box::~TXmlJP2Box() {}


TColorSpecificationJP2Box::TColorSpecificationJP2Box(
    ui8 meth
    , ui8 prec
    , ui8 approx
    , std::optional<ui32> enumCS
    , std::optional<std::vector<ui8>> icc
) : TJP2Box{EJP2BoxType::JP2H_COLOR_SPECIFICATION}
    , Meth_{meth}
    , Prec_{prec}
    , Approx_{approx}
    , EnumeratedColorSpace_{enumCS}
    , ICC_{icc}
    {}

auto TColorSpecificationJP2Box::GetMeth() const -> ui8 {
    return Meth_;
}

auto TColorSpecificationJP2Box::GetPrec() const -> ui8 {
    return Prec_;
}

auto TColorSpecificationJP2Box::GetApprox() const -> ui8 {
    return Approx_;
}

auto TColorSpecificationJP2Box::GetEnumeratedColorSpace() const -> std::optional<ui32> {
    return EnumeratedColorSpace_;
}

auto TColorSpecificationJP2Box::GetICCProfile() const -> std::optional<std::vector<ui8>> {
    return ICC_;
}

std::string TColorSpecificationJP2Box::ToString() const {
    return fmt::format("Color specification box: meth is {}, prec is {}", Meth_, Prec_);
}

TColorSpecificationJP2Box::~TColorSpecificationJP2Box() {}


TCodeStreamJP2Box::TCodeStreamJP2Box(const std::vector<ui8>& data)
    : TJP2Box{EJP2BoxType::CODE_STREAM_BOX}
    , Code_{data}
    {}

auto TCodeStreamJP2Box::GetCode() const -> const std::vector<ui8>& {
    return Code_;
}

std::string TCodeStreamJP2Box::ToString() const {
    return fmt::format("Codestream wiht {} bytes of content", Code_.size());
}

TCodeStreamJP2Box::~TCodeStreamJP2Box() {}

} // namespace kimp::jpeg2000::jp2
