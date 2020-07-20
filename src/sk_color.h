#pragma once

#include <stdlib.h>

/** \file SkColor.h
    Types, consts, functions, and macros for colors.
*/

/** 8-bit type for an alpha value. 255 is 100% opaque, zero is 100% transparent.
*/
typedef uint8_t SkAlpha;

/** 32-bit ARGB color value, unpremultiplied. Color components are always in
    a known order. This is different from SkPMColor, which has its bytes in a configuration
    dependent order, to match the format of kBGRA_8888_SkColorType bitmaps. SkColor
    is the type used to specify colors in SkPaint and in gradients.
    Color that is premultiplied has the same component values as color
    that is unpremultiplied if alpha is 255, fully opaque, although may have the
    component values in a different order.
*/
typedef uint32_t SkColor;

enum class SkColorChannel {
    kR,  // the red channel
    kG,  // the green channel
    kB,  // the blue channel
    kA,  // the alpha channel

    kLastEnum = kA,
};

/** Used to represent the channels available in a color type or texture format as a mask. */
enum SkColorChannelFlag : uint32_t {
    kRed_SkColorChannelFlag    = 1 << static_cast<uint32_t>(SkColorChannel::kR),
    kGreen_SkColorChannelFlag  = 1 << static_cast<uint32_t>(SkColorChannel::kG),
    kBlue_SkColorChannelFlag   = 1 << static_cast<uint32_t>(SkColorChannel::kB),
    kAlpha_SkColorChannelFlag  = 1 << static_cast<uint32_t>(SkColorChannel::kA),
    kGray_SkColorChannelFlag   = 0x10,
    // Convenience values
    kRG_SkColorChannelFlags    = kRed_SkColorChannelFlag | kGreen_SkColorChannelFlag,
    kRGB_SkColorChannelFlags   = kRG_SkColorChannelFlags | kBlue_SkColorChannelFlag,
    kRGBA_SkColorChannelFlags  = kRGB_SkColorChannelFlags | kAlpha_SkColorChannelFlag,
};
static_assert(0 == (kGray_SkColorChannelFlag & kRGBA_SkColorChannelFlags), "bitfield conflict");