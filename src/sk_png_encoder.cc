#include <cassert>
#include <vector>
#include <string>

#include <sk_png_encoder.h>
#include <sk_image_encoder_fns.h>
#include <sk_image_encoder_private.h>
#include <sk_msan.h>

#include <skcms.h>
#include <png.h>

static_assert(PNG_FILTER_NONE  == (int)SkPngEncoder::FilterFlag::kNone,  "Skia libpng filter err.");
static_assert(PNG_FILTER_SUB   == (int)SkPngEncoder::FilterFlag::kSub,   "Skia libpng filter err.");
static_assert(PNG_FILTER_UP    == (int)SkPngEncoder::FilterFlag::kUp,    "Skia libpng filter err.");
static_assert(PNG_FILTER_AVG   == (int)SkPngEncoder::FilterFlag::kAvg,   "Skia libpng filter err.");
static_assert(PNG_FILTER_PAETH == (int)SkPngEncoder::FilterFlag::kPaeth, "Skia libpng filter err.");
static_assert(PNG_ALL_FILTERS  == (int)SkPngEncoder::FilterFlag::kAll,   "Skia libpng filter err.");

static constexpr bool kSuppressPngEncodeWarnings = true;

static void sk_error_fn(png_structp png_ptr, png_const_charp msg) {
    if (!kSuppressPngEncodeWarnings) {
        printf("libpng encode error: %s\n", msg);
    }

    longjmp(png_jmpbuf(png_ptr), 1);
}

static void sk_write_fn(png_structp png_ptr, png_bytep data, png_size_t len) {
    SkWStream* stream = (SkWStream*)png_get_io_ptr(png_ptr);
    if (!stream->write(data, len)) {
        png_error(png_ptr, "sk_write_fn cannot write to stream");
    }
}

class SkPngEncoderMgr final {
public:

    /*
     * Create the decode manager
     * Does not take ownership of stream
     */
    static std::unique_ptr<SkPngEncoderMgr> Make(SkWStream* stream);

    bool setHeader(const SkImageInfo& srcInfo, const SkPngEncoder::Options& options);
    bool setColorSpace(const SkImageInfo& info);
    bool writeInfo(const SkImageInfo& srcInfo);
    void chooseProc(const SkImageInfo& srcInfo);

    png_structp pngPtr() { return fPngPtr; }
    png_infop infoPtr() { return fInfoPtr; }
    int pngBytesPerPixel() const { return fPngBytesPerPixel; }
    transform_scanline_proc proc() const { return fProc; }

    ~SkPngEncoderMgr() {
        png_destroy_write_struct(&fPngPtr, &fInfoPtr);
    }

private:

    SkPngEncoderMgr(png_structp pngPtr, png_infop infoPtr)
        : fPngPtr(pngPtr)
        , fInfoPtr(infoPtr)
    {}

    png_structp             fPngPtr;
    png_infop               fInfoPtr;
    int                     fPngBytesPerPixel;
    transform_scanline_proc fProc;
};

std::unique_ptr<SkPngEncoderMgr> SkPngEncoderMgr::Make(SkWStream* stream) {
    png_structp pngPtr =
            png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, sk_error_fn, nullptr);
    if (!pngPtr) {
        return nullptr;
    }

    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr) {
        png_destroy_write_struct(&pngPtr, nullptr);
        return nullptr;
    }

    png_set_write_fn(pngPtr, (void*)stream, sk_write_fn, nullptr);
    return std::unique_ptr<SkPngEncoderMgr>(new SkPngEncoderMgr(pngPtr, infoPtr));
}

bool SkPngEncoderMgr::setHeader(const SkImageInfo& srcInfo, const SkPngEncoder::Options& options) {
    if (setjmp(png_jmpbuf(fPngPtr))) {
        return false;
    }

    int pngColorType;
    png_color_8 sigBit;
    int bitDepth = 8;
    switch (srcInfo.colorType()) {
        case kBGRA_8888_SkColorType:
            sigBit.red = 8;
            sigBit.green = 8;
            sigBit.blue = 8;
            sigBit.alpha = 8;
            pngColorType = srcInfo.isOpaque() ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA;
            fPngBytesPerPixel = srcInfo.isOpaque() ? 3 : 4;
            break;
        default:
            return false;
    }

    png_set_IHDR(fPngPtr, fInfoPtr, srcInfo.width(), srcInfo.height(),
                 bitDepth, pngColorType,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);
    png_set_sBIT(fPngPtr, fInfoPtr, &sigBit);

    int filters = (int)options.fFilterFlags & (int)SkPngEncoder::FilterFlag::kAll;
    assert(filters == (int)options.fFilterFlags);
    png_set_filter(fPngPtr, PNG_FILTER_TYPE_BASE, filters);

    int zlibLevel = std::min(std::max(0, options.fZLibLevel), 9);
    assert(zlibLevel == options.fZLibLevel);
    png_set_compression_level(fPngPtr, zlibLevel);

    // Set comments in tEXt chunk
    const SkDataTable* comments = options.fComments;
    if (comments != nullptr) {
        std::vector<png_text> png_texts(comments->count());
        std::vector<std::string> clippedKeys;
        for (int i = 0; i < comments->count() / 2; ++i) {
            const char* keyword;
            const char* originalKeyword = comments->atStr(2 * i);
            const char* text = comments->atStr(2 * i + 1);
            if (strlen(originalKeyword) <= PNG_KEYWORD_MAX_LENGTH) {
                keyword = originalKeyword;
            } else {
                printf("PNG tEXt keyword should be no longer than %d.",
                        PNG_KEYWORD_MAX_LENGTH);
                clippedKeys.emplace_back(originalKeyword, PNG_KEYWORD_MAX_LENGTH);
                keyword = clippedKeys.back().c_str();
            }
            // It seems safe to convert png_const_charp to png_charp for key/text,
            // and we don't have to provide text_length and other fields as we're providing
            // 0-terminated c_str with PNG_TEXT_COMPRESSION_NONE (no compression, no itxt).
            png_texts[i].compression = PNG_TEXT_COMPRESSION_NONE;
            png_texts[i].key = (png_charp)keyword;
            png_texts[i].text = (png_charp)text;
        }
        png_set_text(fPngPtr, fInfoPtr, png_texts.data(), png_texts.size());
    }

    return true;
}

static transform_scanline_proc choose_proc(const SkImageInfo& info) {
    switch (info.colorType()) {
        case kUnknown_SkColorType:
            break;

        case kBGRA_8888_SkColorType:
            switch (info.alphaType()) {
                case kPremul_SkAlphaType:
                    return transform_scanline_bgrA;
                default:
                    assert(false);
                    return nullptr;
            }
    }
    assert(false);
    return nullptr;
}

static void set_icc(png_structp png_ptr, png_infop info_ptr, const SkImageInfo& info) {
    return;
}

bool SkPngEncoderMgr::setColorSpace(const SkImageInfo& info) {
    if (setjmp(png_jmpbuf(fPngPtr))) {
        return false;
    }

    return true;
}

bool SkPngEncoderMgr::writeInfo(const SkImageInfo& srcInfo) {
    if (setjmp(png_jmpbuf(fPngPtr))) {
        return false;
    }

    png_write_info(fPngPtr, fInfoPtr);
    return true;
}

void SkPngEncoderMgr::chooseProc(const SkImageInfo& srcInfo) {
    fProc = choose_proc(srcInfo);
}

std::unique_ptr<SkEncoder> SkPngEncoder::Make(SkWStream* dst, const SkPixmap& src,
                                              const Options& options) {
    if (!SkPixmapIsValid(src)) {
        return nullptr;
    }

    std::unique_ptr<SkPngEncoderMgr> encoderMgr = SkPngEncoderMgr::Make(dst);
    if (!encoderMgr) {
        return nullptr;
    }

    if (!encoderMgr->setHeader(src.info(), options)) {
        return nullptr;
    }

    if (!encoderMgr->setColorSpace(src.info())) {
        return nullptr;
    }

    if (!encoderMgr->writeInfo(src.info())) {
        return nullptr;
    }

    encoderMgr->chooseProc(src.info());

    return std::unique_ptr<SkPngEncoder>(new SkPngEncoder(std::move(encoderMgr), src));
}

SkPngEncoder::SkPngEncoder(std::unique_ptr<SkPngEncoderMgr> encoderMgr, const SkPixmap& src)
    : INHERITED(src, encoderMgr->pngBytesPerPixel() * src.width())
    , fEncoderMgr(std::move(encoderMgr))
{}

SkPngEncoder::~SkPngEncoder() {}

bool SkPngEncoder::onEncodeRows(int numRows) {
    if (setjmp(png_jmpbuf(fEncoderMgr->pngPtr()))) {
        return false;
    }

    const void* srcRow = fSrc.addr(0, fCurrRow);
    for (int y = 0; y < numRows; y++) {
        sk_msan_assert_initialized(srcRow,
                                   (const uint8_t*)srcRow + (fSrc.width() << fSrc.shiftPerPixel()));
        fEncoderMgr->proc()((char*)fStorage.get(),
                            (const char*)srcRow,
                            fSrc.width(),
                            SkColorTypeBytesPerPixel(fSrc.colorType()));

        png_bytep rowPtr = (png_bytep) fStorage.get();
        png_write_rows(fEncoderMgr->pngPtr(), &rowPtr, 1);
        srcRow = SkTAddOffset<const void>(srcRow, fSrc.rowBytes());
    }

    fCurrRow += numRows;
    if (fCurrRow == fSrc.height()) {
        png_write_end(fEncoderMgr->pngPtr(), fEncoderMgr->infoPtr());
    }

    return true;
}

bool SkPngEncoder::Encode(SkWStream* dst, const SkPixmap& src, const Options& options) {
    auto encoder = SkPngEncoder::Make(dst, src, options);
    return encoder.get() && encoder->encodeRows(src.height());
}
