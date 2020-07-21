#include <simple_bgra_8888_transformer.h>

#include <sk_image_encoder_fns.h>
#include <sk_image_info.h>
#include <sk_image_encoder_private.h>
#include <sk_msan.h>

static transform_scanline_proc choose_proc(const SkImageInfo& info) {
    switch (info.colorType()) {
        case kUnknown_SkColorType:
            break;

        case kBGRA_8888_SkColorType:
            switch (info.alphaType()) {
                case kPremul_SkAlphaType:
                    return transform_scanline_bgrA_same;
                default:
                    assert(false);
                    return nullptr;
            }
    }
    
    assert(false);
    return nullptr;
}

std::unique_ptr<SkEncoder> SimpleBGRA8888Transformer::Make(const SkPixmap& src, SkWStream* dst, 
                                                            const SkImageInfo& info) {
    if (!SkPixmapIsValid(src)) {
        return nullptr;
    }

    auto ret = std::unique_ptr<SimpleBGRA8888Transformer>(new SimpleBGRA8888Transformer(src, dst));
    
    ret->chooseProc(info);
    
    return ret;
}

SimpleBGRA8888Transformer::SimpleBGRA8888Transformer(const SkPixmap& src, SkWStream* dst)
    : INHERITED(src, 4 * src.width())
    , fDst(dst)
{}

void SimpleBGRA8888Transformer::chooseProc(const SkImageInfo& info) {
    fProc = choose_proc(info);
}

SimpleBGRA8888Transformer::~SimpleBGRA8888Transformer() {}

bool SimpleBGRA8888Transformer::onEncodeRows(int numRows) {
    const void* srcRow = fSrc.addr(0, fCurrRow);
    for (int y = 0; y < numRows; y++) {
        sk_msan_assert_initialized(srcRow,
                                   (const uint8_t*)srcRow + (fSrc.width() << fSrc.shiftPerPixel()));
        fProc((char*)fStorage.get(),
                            (const char*)srcRow,
                            fSrc.width(),
                            SkColorTypeBytesPerPixel(fSrc.colorType()));

        if (!fDst->write(fStorage.get(), 4 * fSrc.width())) {
            perror("simple_bgra_8888_transformer: cannot write to stream");
        }

        srcRow = SkTAddOffset<const void>(srcRow, fSrc.rowBytes());
    }

    fCurrRow += numRows;
    return true;
}

bool SimpleBGRA8888Transformer::Encode(const SkPixmap& src, SkWStream* dst, const SkImageInfo& info) {
    auto encoder = SimpleBGRA8888Transformer::Make(src, dst, info);
    return encoder.get() && encoder->encodeRows(src.height());
}
