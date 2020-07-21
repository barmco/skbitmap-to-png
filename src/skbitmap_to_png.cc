#include <stdlib.h>

#include <vector>

#include <sk_image_info.h>
#include <sk_pixmap.h>
#include <vector_wstream.h>

#include <png_codec.h>
#include <simple_bgra_8888_transformer.h>

#include <skbitmap_to_png.h>

extern "C" TransformResult transform_to_png(int width, int height, size_t size, void *buf) {
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

    auto pixels = SkPixmap(info, buf, info.minRowBytes());
    auto encoded = new std::vector<unsigned char>;

    if(!PNGCodec::FastEncodeBGRASkBitmap(pixels, encoded))
        return { nullptr, 0 };
    
    return {
        reinterpret_cast<void *>(encoded),
        encoded->data(),
        encoded->size()
    };
}

extern "C" TransformResult transform_to_bgra8888(int width, int height, size_t size, void *buf) {
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

    auto pixels = SkPixmap(info, buf, info.minRowBytes());
    auto encoded = new std::vector<unsigned char>;

    encoded->clear();
    VectorWStream dst(encoded);

    if(!SimpleBGRA8888Transformer::Encode(pixels, &dst, info))
        return { nullptr, 0 };

    return {
        reinterpret_cast<void *>(encoded),
        encoded->data(),
        encoded->size()
    };
}

extern "C" size_t compute_min_bytesize(int width, int height) {
    auto info = SkImageInfo::MakeN32(width, height, kPremul_SkAlphaType);
    return info.computeMinByteSize();
}

extern "C" void memfree(void *handle) {
    auto origin = reinterpret_cast<std::vector<unsigned char> *>(handle);
    delete origin;
}
