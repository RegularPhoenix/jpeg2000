#pragma once

#include <jpeg2000/utils/num.hpp>

#include <pugixml.hpp>

#include <optional>
#include <vector>

namespace kimp::jpeg2000::jp2 {

enum class EJP2BoxType : ui32 {
    SIGNATURE_BOX = 0x6a502020,
    FILE_TYPE_BOX = 0x66747970,
    JP2H_CONTAINER = 0x6a703268,
    JP2H_IMAGE_HEADER = 0x69686472,
    JP2H_COLOR_SPECIFICATION = 0x636f6c72,
    CODE_STREAM_BOX = 0x6a703263,
    XML_BOX = 0x786d6c20,
    UNKNOWN
};

abstract class TJP2Box {
public:
    TJP2Box(EJP2BoxType);
    auto GetType() const -> EJP2BoxType;
    virtual std::string ToString() const = 0;
    virtual ~TJP2Box();
private:
    const EJP2BoxType BoxType_;
};

class TSignatureJP2Box : public TJP2Box {
public:
    TSignatureJP2Box(ui32 signature);

    auto GetSignature() const -> ui32;
    auto IsSignatureValid() const -> bool;

    virtual std::string ToString() const override;

    virtual ~TSignatureJP2Box();

public:
    static const ui32 VALID_SIGNATURE = 0x0d0a870a;

private:
    const ui32 Signature_;
};

class TFileTypeJP2Box : public TJP2Box {
public:
    TFileTypeJP2Box(ui32 brand, ui32 minorVersion, const std::vector<ui32>& compatibilities);

    auto GetBrand() const -> ui32;
    auto GetMinorVersion() const -> ui32;
    auto GetCompatibilities() const -> const std::vector<ui32>&;

    auto IsJPEG2000() const -> bool;

    virtual std::string ToString() const override;

    virtual ~TFileTypeJP2Box();

public:
    static const ui32 JPEG2000_BRAND = 0x6a703220;

private:
    const ui32 Brand_;
    const ui32 MinorVersion_;
    const std::vector<ui32> Compatibilities_;
};

class THeaderContainerJP2Box : public TJP2Box {
public:
    THeaderContainerJP2Box(const std::vector<std::shared_ptr<TJP2Box>>& children);

    auto GetChildren() const -> const std::vector<std::shared_ptr<TJP2Box>>&;

    virtual std::string ToString() const override;

    virtual ~THeaderContainerJP2Box();

private:
    const std::vector<std::shared_ptr<TJP2Box>> Children_;
};

class TImageHeaderJP2Box : public TJP2Box {
public:
    TImageHeaderJP2Box(ui32 height, ui32 width, ui16 nc, ui8 bpc, ui8 c, ui8 unkC, ui8 ipr);

    auto GetHeight() const -> ui32;
    auto GetWidth() const -> ui32;
    auto GetNumberOfComponents() const -> ui16;
    auto GetBitsPerComponent() const -> ui8;
    auto GetCompressionType() const -> ui8;
    auto IsColorspaceUnknown() const -> bool;
    auto IsIntellectualProperty() const -> bool;

    auto IsJPEG2000() const -> bool;

    virtual std::string ToString() const override;

    virtual ~TImageHeaderJP2Box();

public:
    static const ui8 JPEG2000_COMPRESSION_TYPE = 0x07;

private:
    const ui32 Height_;
    const ui32 Width_;
    const ui16 NumberOfComponents_;
    const ui8 BitsPerComponent_;
    const ui8 CompessionType_;
    const ui8 IsColorspaceUnknown_;
    const ui8 IsIntellectualProperty_;
};

class TXmlJP2Box : public TJP2Box {
public:
    TXmlJP2Box(pugi::xml_document&& doc);

    auto GetXmlDoc() const -> const pugi::xml_document&;

    virtual std::string ToString() const override;

    virtual ~TXmlJP2Box();

private:
    const pugi::xml_document XmlDoc_;
};

class TColorSpecificationJP2Box : public TJP2Box {
public:
    TColorSpecificationJP2Box(ui8 meth, ui8 prec, ui8 approx, std::optional<ui32> enumCS, std::optional<std::vector<ui8>> icc);

    auto GetMeth() const -> ui8;
    auto GetPrec() const -> ui8;
    auto GetApprox() const -> ui8;
    auto GetEnumeratedColorSpace() const -> std::optional<ui32>;
    auto GetICCProfile() const -> std::optional<std::vector<ui8>>;

    virtual std::string ToString() const override;

    virtual ~TColorSpecificationJP2Box();

private:
    const ui8 Meth_;
    const ui8 Prec_;
    const ui8 Approx_;
    const std::optional<ui32> EnumeratedColorSpace_;
    const std::optional<std::vector<ui8>> ICC_;
};

class TCodeStreamJP2Box : public TJP2Box {
public:
    TCodeStreamJP2Box(const std::vector<ui8>& data);

    auto GetCode() const -> const std::vector<ui8>&;

    virtual std::string ToString() const override;

    virtual ~TCodeStreamJP2Box();

private:
    const std::vector<ui8> Code_;
};

} // namespace kimp::jpeg2000:jp2
