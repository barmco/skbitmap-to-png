#pragma once

#include <memory>

#include <sk_pixmap.h>
#include <sk_stream.h>
#include <sk_encoder.h>
#include <sk_image_encoder_fns.h>

class SimpleBGRA8888Transformer : public SkEncoder {
public:
    /**
     *  Encode the |src| pixels to the |dst| stream.
     *  |options| may be used to control the encoding behavior.
     *
     *  Returns true on success.  Returns false on an invalid or unsupported |src|.
     */
    static bool Encode(const SkPixmap& src, SkWStream* dst, const SkImageInfo& info);

    static std::unique_ptr<SkEncoder> Make(const SkPixmap& src, SkWStream* dst, const SkImageInfo& info);

    void chooseProc(const SkImageInfo& srcInfo);

    ~SimpleBGRA8888Transformer() override;

protected:
    bool onEncodeRows(int numRows) override;

    SimpleBGRA8888Transformer(const SkPixmap& src, SkWStream *dst);

    typedef SkEncoder INHERITED;

private:
    transform_scanline_proc fProc;
    SkWStream *fDst;
};