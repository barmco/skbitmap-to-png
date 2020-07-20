#include <stdlib.h>

#include <vector>

#include <sk_image_info.h>
#include <sk_pixmap.h>
#include <png_codec.h>

#include <skbitmap_to_png.h>

extern "C" {
    TransformResult transform(int width, int height, size_t size, void *buf) {
        auto info = SkImageInfo::MakeN32(width, height, kPremul_SkAlphaType);
        auto size_bytes = info.computeMinByteSize();

        if(size_bytes != size) {
            perror("invalid buffer size");
            return { nullptr, 0 };
        }

        if(width == 0 || height == 0) {
            perror("invalid width or height given");
            return { nullptr, 0 };
        }

        auto pixels = SkPixmap(info, buf, size);
        std::vector<unsigned char> encoded;

        if(!PNGCodec::FastEncodeBGRASkBitmap(pixels, &encoded))
            return { nullptr, 0 };
        
        return {
            encoded.data(),
            encoded.size()
        };
    }
}