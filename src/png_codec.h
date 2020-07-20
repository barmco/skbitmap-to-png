#pragma once

#include <stddef.h>

#include <string>
#include <vector>

#include <sk_pixmap.h>

// Interface for encoding and decoding PNG data. This is a wrapper around
// libpng, which has an inconvenient interface for callers. This is currently
// designed for use in tests only (where we control the files), so the handling
// isn't as robust as would be required for a browser (see Decode() for more).
// WebKit has its own more complicated PNG decoder which handles, among other
// things, partially downloaded data.
class PNGCodec {
 public:
  static constexpr int DEFAULT_ZLIB_COMPRESSION = 6;

  enum ColorFormat {
    // 4 bytes per pixel, in BGRA order in memory regardless of endianness.
    // This is the default Windows DIB order.
    FORMAT_BGRA,

    // SkBitmap format. For Encode() kN32_SkColorType (4 bytes per pixel) and
    // kAlpha_8_SkColorType (1 byte per pixel) formats are supported.
    // kAlpha_8_SkColorType gets encoded into a grayscale PNG treating alpha as
    // the color intensity. For Decode() kN32_SkColorType is always used.
    FORMAT_SkBitmap
  };

  // Represents a comment in the tEXt ancillary chunk of the png.
  struct Comment {
    Comment(const std::string& k, const std::string& t);
    ~Comment();

    std::string key;
    std::string text;
  };


  // Call PNGCodec::Encode on the supplied SkBitmap |input|. The difference
  // between this and the previous method is that this restricts compression to
  // zlib q1, which is just rle encoding.
  static bool FastEncodeBGRASkBitmap(const SkPixmap& input, std::vector<unsigned char>* output);
};