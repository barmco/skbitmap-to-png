#include <vector>

#include <zlib.h>

#include <sk_pixmap.h>
#include <sk_png_encoder.h>

#include <png_codec.h>
#include <vector_wstream.h>

static void AddComments(SkPngEncoder::Options& options,
                        const std::vector<PNGCodec::Comment>& comments) {
  std::vector<const char*> comment_pointers;
  std::vector<size_t> comment_sizes;
  for (const auto& comment : comments) {
    comment_pointers.push_back(comment.key.c_str());
    comment_pointers.push_back(comment.text.c_str());
    comment_sizes.push_back(comment.key.length() + 1);
    comment_sizes.push_back(comment.text.length() + 1);
  }
  options.fComments = SkDataTable::MakeCopyArrays(
      (void const* const*)comment_pointers.data(), comment_sizes.data(),
      static_cast<int>(comment_pointers.size()));
}

static bool EncodeSkPixmap(const SkPixmap& src,
                           const std::vector<PNGCodec::Comment>& comments,
                           std::vector<unsigned char>* output,
                           int zlib_level) {
  output->clear();
  VectorWStream dst(output);

  SkPngEncoder::Options options;
  AddComments(options, comments);
  options.fZLibLevel = zlib_level;
  return SkPngEncoder::Encode(&dst, src, options);
}

// static
bool PNGCodec::FastEncodeBGRASkBitmap(const SkPixmap& input, std::vector<unsigned char>* output) {
  return EncodeSkPixmap(input, std::vector<PNGCodec::Comment>(), output, Z_BEST_SPEED);
}

PNGCodec::Comment::Comment(const std::string& k, const std::string& t)
    : key(k), text(t) {
}

PNGCodec::Comment::~Comment() {
}