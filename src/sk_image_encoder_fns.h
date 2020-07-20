#pragma once

#include <skcms.h>

typedef void (*transform_scanline_proc)(char* dst, const char* src, int width, int bpp);

static void skcms(char* dst, const char* src, int n,
                  skcms_PixelFormat srcFmt, skcms_AlphaFormat srcAlpha,
                  skcms_PixelFormat dstFmt, skcms_AlphaFormat dstAlpha) {
    skcms_Transform(src, srcFmt, srcAlpha, nullptr,
                                   dst, dstFmt, dstAlpha, nullptr, n);
}

static inline void transform_scanline_bgrA(char* dst, const char* src, int width, int) {
    skcms(dst, src, width,
          skcms_PixelFormat_BGRA_8888, skcms_AlphaFormat_PremulAsEncoded,
          skcms_PixelFormat_RGBA_8888, skcms_AlphaFormat_Unpremul);
}

static inline void transform_scanline_BGRA(char* dst, const char* src, int width, int) {
    skcms(dst, src, width,
          skcms_PixelFormat_BGRA_8888, skcms_AlphaFormat_Unpremul,
          skcms_PixelFormat_RGBA_8888, skcms_AlphaFormat_Unpremul);
}