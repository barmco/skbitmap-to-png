#pragma once

#include <algorithm>

#include <sk_size.h>
#include <sk_color.h>
#include <sk_math.h>
#include <sk_template_fits_in.h>

/** \enum SkImageInfo::SkAlphaType
    Describes how to interpret the alpha component of a pixel. A pixel may
    be opaque, or alpha, describing multiple levels of transparency.
    In simple blending, alpha weights the draw color and the destination
    color to create a new color. If alpha describes a weight from zero to one:
    new color = draw color * alpha + destination color * (1 - alpha)
    In practice alpha is encoded in two or more bits, where 1.0 equals all bits set.
    RGB may have alpha included in each component value; the stored
    value is the original RGB multiplied by alpha. Premultiplied color
    components improve performance.
*/
enum SkAlphaType {
    kUnknown_SkAlphaType,                          //!< uninitialized
    kPremul_SkAlphaType,                           //!< pixel components are premultiplied by alpha
    kLastEnum_SkAlphaType = kPremul_SkAlphaType, //!< last valid value
};

///////////////////////////////////////////////////////////////////////////////

/** \enum SkImageInfo::SkColorType
    Describes how pixel bits encode color. A pixel may be an alpha mask, a grayscale, RGB, or ARGB.
    kN32_SkColorType selects the native 32-bit ARGB format for the current configuration. This can
    lead to inconsistent results across platforms, so use with caution.
*/
enum SkColorType {
    kUnknown_SkColorType,      //!< uninitialized
    kBGRA_8888_SkColorType,    //!< pixel with 8 bits for blue, green, red, alpha; in 32-bit word
    kN32_SkColorType          = kBGRA_8888_SkColorType,//!< native 32-bit BGRA encoding
};

/** Returns the number of bytes required to store a pixel, including unused padding.
    Returns zero if ct is kUnknown_SkColorType or invalid.
    @return    bytes per pixel
*/
int SkColorTypeBytesPerPixel(SkColorType ct);

/** \struct SkColorInfo
    Describes pixel and encoding. SkImageInfo can be created from SkColorInfo by
    providing dimensions.
    It encodes how pixel bits describe alpha, transparency; color components red, blue,
    and green; and SkColorSpace, the range and linearity of colors.
*/
class SkColorInfo {
public:
    /** Creates an SkColorInfo with kUnknown_SkColorType, kUnknown_SkAlphaType,
        and no SkColorSpace.
        @return  empty SkImageInfo
    */
    SkColorInfo() = default;

    /** Creates SkColorInfo from SkColorType ct, SkAlphaType at, and optionally SkColorSpace cs.
        If SkColorSpace cs is nullptr and SkColorInfo is part of drawing source: SkColorSpace
        defaults to sRGB, mapping into SkSurface SkColorSpace.
        Parameters are not validated to see if their values are legal, or that the
        combination is supported.
        @return        created SkColorInfo
    */
    SkColorInfo(SkColorType ct, SkAlphaType at)
            : fColorType(ct), fAlphaType(at) {}

    SkColorInfo(const SkColorInfo&) = default;
    SkColorInfo(SkColorInfo&&) = default;

    SkColorInfo& operator=(const SkColorInfo&) = default;
    SkColorInfo& operator=(SkColorInfo&&) = default;

    SkColorType colorType() const { return fColorType; }
    SkAlphaType alphaType() const { return fAlphaType; }

    bool isOpaque() const {
        return false;
    }

    bool gammaCloseToSRGB() const { return false; }

    /** Does other represent the same color type, alpha type, and color space? */
    bool operator==(const SkColorInfo& other) const {
        return fColorType == other.fColorType && fAlphaType == other.fAlphaType;
    }

    /** Does other represent a different color type, alpha type, or color space? */
    bool operator!=(const SkColorInfo& other) const { return !(*this == other); }

    /** Creates SkColorInfo with same SkColorType, SkColorSpace, with SkAlphaType set
        to newAlphaType.
        Created SkColorInfo contains newAlphaType even if it is incompatible with
        SkColorType, in which case SkAlphaType in SkColorInfo is ignored.
    */
    SkColorInfo makeAlphaType(SkAlphaType newAlphaType) const {
        return SkColorInfo(this->colorType(), newAlphaType);
    }

    /** Creates new SkColorInfo with same SkAlphaType, SkColorSpace, with SkColorType
        set to newColorType.
    */
    SkColorInfo makeColorType(SkColorType newColorType) const {
        return SkColorInfo(newColorType, this->alphaType());
    }

    /** Returns number of bytes per pixel required by SkColorType.
        Returns zero if colorType() is kUnknown_SkColorType.
        @return  bytes in pixel
        example: https://fiddle.skia.org/c/@ImageInfo_bytesPerPixel
    */
    int bytesPerPixel() const;

    /** Returns bit shift converting row bytes to row pixels.
        Returns zero for kUnknown_SkColorType.
        @return  one of: 0, 1, 2, 3, 4; left shift to convert pixels to bytes
        example: https://fiddle.skia.org/c/@ImageInfo_shiftPerPixel
    */
    int shiftPerPixel() const;

private:
    SkColorType fColorType = SkColorType::kUnknown_SkColorType;
    SkAlphaType fAlphaType = SkAlphaType::kUnknown_SkAlphaType;
};

/** \struct SkImageInfo
    Describes pixel dimensions and encoding. SkBitmap, SkImage, PixMap, and SkSurface
    can be created from SkImageInfo. SkImageInfo can be retrieved from SkBitmap and
    SkPixmap, but not from SkImage and SkSurface. For example, SkImage and SkSurface
    implementations may defer pixel depth, so may not completely specify SkImageInfo.
    SkImageInfo contains dimensions, the pixel integral width and height. It encodes
    how pixel bits describe alpha, transparency; color components red, blue,
    and green; and SkColorSpace, the range and linearity of colors.
*/
struct SkImageInfo {
public:

    /** Creates an empty SkImageInfo with kUnknown_SkColorType, kUnknown_SkAlphaType,
        a width and height of zero, and no SkColorSpace.
        @return  empty SkImageInfo
    */
    SkImageInfo() = default;

    /** Creates SkImageInfo from integral dimensions width and height, SkColorType ct,
        SkAlphaType at, and optionally SkColorSpace cs.
        If SkColorSpace cs is nullptr and SkImageInfo is part of drawing source: SkColorSpace
        defaults to sRGB, mapping into SkSurface SkColorSpace.
        Parameters are not validated to see if their values are legal, or that the
        combination is supported.
        @param width   pixel column count; must be zero or greater
        @param height  pixel row count; must be zero or greater
        @param cs      range of colors; may be nullptr
        @return        created SkImageInfo
    */
    static SkImageInfo Make(int width, int height, SkColorType ct, SkAlphaType at) {
        return SkImageInfo({width, height}, { ct, at });
    }
    static SkImageInfo Make(SkISize dimensions, SkColorType ct, SkAlphaType at) {
        return SkImageInfo(dimensions, { ct, at });
    }

    /** Creates SkImageInfo from integral dimensions width and height, kN32_SkColorType,
        SkAlphaType at, and optionally SkColorSpace cs. kN32_SkColorType will equal either
        kBGRA_8888_SkColorType or kRGBA_8888_SkColorType, whichever is optimal.
        If SkColorSpace cs is nullptr and SkImageInfo is part of drawing source: SkColorSpace
        defaults to sRGB, mapping into SkSurface SkColorSpace.
        Parameters are not validated to see if their values are legal, or that the
        combination is supported.
        @param width   pixel column count; must be zero or greater
        @param height  pixel row count; must be zero or greater
        @param cs      range of colors; may be nullptr
        @return        created SkImageInfo
    */
    static SkImageInfo MakeN32(int width, int height, SkAlphaType at) {
        return Make({width, height}, SkColorType::kN32_SkColorType, at);
    }

     /** Creates SkImageInfo from integral dimensions width and height, kUnknown_SkColorType,
        kUnknown_SkAlphaType, with SkColorSpace set to nullptr.
        Returned SkImageInfo as part of source does not draw, and as part of destination
        can not be drawn to.
        @param width   pixel column count; must be zero or greater
        @param height  pixel row count; must be zero or greater
        @return        created SkImageInfo
    */
    static SkImageInfo MakeUnknown(int width, int height) {
        return Make({width, height}, SkColorType::kUnknown_SkColorType, SkAlphaType::kUnknown_SkAlphaType);
    }

    /** Creates SkImageInfo from integral dimensions width and height set to zero,
        kUnknown_SkColorType, kUnknown_SkAlphaType, with SkColorSpace set to nullptr.
        Returned SkImageInfo as part of source does not draw, and as part of destination
        can not be drawn to.
        @return  created SkImageInfo
    */
    static SkImageInfo MakeUnknown() {
        return MakeUnknown(0, 0);
    }

    /** Returns pixel count in each row.
        @return  pixel width
    */
    int width() const { return fDimensions.width(); }

    /** Returns pixel row count.
        @return  pixel height
    */
    int height() const { return fDimensions.height(); }

    SkColorType colorType() const { return fColorInfo.colorType(); }

    SkAlphaType alphaType() const { return fColorInfo.alphaType(); }

    /** Returns if SkImageInfo describes an empty area of pixels by checking if either
        width or height is zero or smaller.
        @return  true if either dimension is zero or smaller
    */
    bool isEmpty() const { return fDimensions.isEmpty(); }

    /** Returns the dimensionless SkColorInfo that represents the same color type,
        alpha type, and color space as this SkImageInfo.
     */
    const SkColorInfo& colorInfo() const { return fColorInfo; }

    /** Returns true if SkAlphaType is set to hint that all pixels are opaque; their
        alpha value is implicitly or explicitly 1.0. If true, and all pixels are
        not opaque, Skia may draw incorrectly.
        Does not check if SkColorType allows alpha, or if any pixel value has
        transparency.
        @return  true if SkAlphaType is kOpaque_SkAlphaType
    */
    bool isOpaque() const { return fColorInfo.isOpaque(); }

    /** Returns SkISize { width(), height() }.
        @return  integral size of width() and height()
    */
    SkISize dimensions() const { return fDimensions; }

    /** Returns true if associated SkColorSpace is not nullptr, and SkColorSpace gamma
        is approximately the same as sRGB.
        This includes the
        @return  true if SkColorSpace gamma is approximately the same as sRGB
    */
    bool gammaCloseToSRGB() const { return fColorInfo.gammaCloseToSRGB(); }


    /** Returns number of bytes per pixel required by SkColorType.
        Returns zero if colorType( is kUnknown_SkColorType.
        @return  bytes in pixel
    */
    int bytesPerPixel() const { return fColorInfo.bytesPerPixel(); }

    /** Returns bit shift converting row bytes to row pixels.
        Returns zero for kUnknown_SkColorType.
        @return  one of: 0, 1, 2, 3; left shift to convert pixels to bytes
    */
    int shiftPerPixel() const { return fColorInfo.shiftPerPixel(); }

    /** Returns minimum bytes per row, computed from pixel width() and SkColorType, which
        specifies bytesPerPixel(). SkBitmap maximum value for row bytes must fit
        in 31 bits.
        @return  width() times bytesPerPixel() as unsigned 64-bit integer
    */
    uint64_t minRowBytes64() const {
        return (uint64_t)sk_64_mul(this->width(), this->bytesPerPixel());
    }

    /** Returns minimum bytes per row, computed from pixel width() and SkColorType, which
        specifies bytesPerPixel(). SkBitmap maximum value for row bytes must fit
        in 31 bits.
        @return  width() times bytesPerPixel() as size_t
    */
    size_t minRowBytes() const {
        uint64_t minRowBytes = this->minRowBytes64();
        if (!SkTFitsIn<int32_t>(minRowBytes)) {
            return 0;
        }
        return (size_t)minRowBytes;
    }

    /** Returns byte offset of pixel from pixel base address.
        Asserts in debug build if x or y is outside of bounds. Does not assert if
        rowBytes is smaller than minRowBytes(), even though result may be incorrect.
        @param x         column index, zero or greater, and less than width()
        @param y         row index, zero or greater, and less than height()
        @param rowBytes  size of pixel row or larger
        @return          offset within pixel array
        example: https://fiddle.skia.org/c/@ImageInfo_computeOffset
    */
    size_t computeOffset(int x, int y, size_t rowBytes) const;

    /** Compares SkImageInfo with other, and returns true if width, height, SkColorType,
        SkAlphaType, and SkColorSpace are equivalent.
        @param other  SkImageInfo to compare
        @return       true if SkImageInfo equals other
    */
    bool operator==(const SkImageInfo& other) const {
        return fDimensions == other.fDimensions && fColorInfo == other.fColorInfo;
    }

    /** Compares SkImageInfo with other, and returns true if width, height, SkColorType,
        SkAlphaType, and SkColorSpace are not equivalent.
        @param other  SkImageInfo to compare
        @return       true if SkImageInfo is not equal to other
    */
    bool operator!=(const SkImageInfo& other) const {
        return !(*this == other);
    }

    /** Returns storage required by pixel array, given SkImageInfo dimensions, SkColorType,
        and rowBytes. rowBytes is assumed to be at least as large as minRowBytes().
        Returns zero if height is zero.
        Returns SIZE_MAX if answer exceeds the range of size_t.
        @param rowBytes  size of pixel row or larger
        @return          memory required by pixel buffer
        example: https://fiddle.skia.org/c/@ImageInfo_computeByteSize
    */
    size_t computeByteSize(size_t rowBytes) const;

    /** Returns storage required by pixel array, given SkImageInfo dimensions, and
        SkColorType. Uses minRowBytes() to compute bytes for pixel row.
        Returns zero if height is zero.
        Returns SIZE_MAX if answer exceeds the range of size_t.
        @return  least memory required by pixel buffer
    */
    size_t computeMinByteSize() const {
        return this->computeByteSize(this->minRowBytes());
    }

    /** Returns true if byteSize equals SIZE_MAX. computeByteSize() and
        computeMinByteSize() return SIZE_MAX if size_t can not hold buffer size.
        @param byteSize  result of computeByteSize() or computeMinByteSize()
        @return          true if computeByteSize() or computeMinByteSize() result exceeds size_t
    */
    static bool ByteSizeOverflowed(size_t byteSize) {
        return SIZE_MAX == byteSize;
    }

    /** Returns true if rowBytes is valid for this SkImageInfo.
        @param rowBytes  size of pixel row including padding
        @return          true if rowBytes is large enough to contain pixel row and is properly
                         aligned
    */
    bool validRowBytes(size_t rowBytes) const {
        if (rowBytes < this->minRowBytes64()) {
            return false;
        }
        int shift = this->shiftPerPixel();
        size_t alignedRowBytes = rowBytes >> shift << shift;
        return alignedRowBytes == rowBytes;
    }

    /** Creates an empty SkImageInfo with kUnknown_SkColorType, kUnknown_SkAlphaType,
        a width and height of zero, and no SkColorSpace.
    */
    void reset() { *this = {}; }

private:
    SkColorInfo fColorInfo;
    SkISize fDimensions = {0, 0};

    SkImageInfo(SkISize dimensions, const SkColorInfo& colorInfo)
            : fColorInfo(colorInfo), fDimensions(dimensions) {}
};