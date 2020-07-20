#include <cassert>

#include <sk_image_info.h>
#include <sk_image_info_private.h>
#include <sk_safe_math.h>

int SkColorTypeBytesPerPixel(SkColorType ct) {
    switch (ct) {
        case kUnknown_SkColorType:            return 0;
        case kBGRA_8888_SkColorType:          return 4;
    }
}

int SkColorInfo::bytesPerPixel() const { return SkColorTypeBytesPerPixel(fColorType); }

int SkColorInfo::shiftPerPixel() const { return SkColorTypeShiftPerPixel(fColorType); }

size_t SkImageInfo::computeByteSize(size_t rowBytes) const {
    if (0 == this->height()) {
        return 0;
    }

    SkSafeMath safe;
    size_t bytes = safe.add(safe.mul(safe.addInt(this->height(), -1), rowBytes),
                            safe.mul(this->width(), this->bytesPerPixel()));
    return safe.ok() ? bytes : SIZE_MAX;
}

size_t SkImageInfo::computeOffset(int x, int y, size_t rowBytes) const {
    assert((unsigned)x < (unsigned)this->width());
    assert((unsigned)y < (unsigned)this->height());
    return SkColorTypeComputeOffset(this->colorType(), x, y, rowBytes);
}