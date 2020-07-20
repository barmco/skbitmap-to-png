#pragma once

#include <sk_types.h>
#include <sk_image_info.h>

static int SkColorTypeShiftPerPixel(SkColorType ct) {
    switch (ct) {
        case kUnknown_SkColorType:            return 0;
        case kBGRA_8888_SkColorType:          return 2;
    }
}

static inline size_t SkColorTypeComputeOffset(SkColorType ct, int x, int y, size_t rowBytes) {
    if (kUnknown_SkColorType == ct) {
        return 0;
    }
    return (size_t)y * rowBytes + ((size_t)x << SkColorTypeShiftPerPixel(ct));
}

/**
 *  Returns true if |info| contains a valid colorType and alphaType.
 */
static inline bool SkColorInfoIsValid(const SkColorInfo& info) {
    return info.colorType() != SkColorType::kUnknown_SkColorType && info.alphaType() != SkAlphaType::kUnknown_SkAlphaType;
}

/**
 *  Returns true if |info| contains a valid combination of width, height and colorInfo.
 */
static inline bool SkImageInfoIsValid(const SkImageInfo& info) {
    if (info.width() <= 0 || info.height() <= 0) {
        return false;
    }

    const int kMaxDimension = SK_MaxS32 >> 2;
    if (info.width() > kMaxDimension || info.height() > kMaxDimension) {
        return false;
    }

    return SkColorInfoIsValid(info.colorInfo());
}