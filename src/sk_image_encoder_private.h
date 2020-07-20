#pragma once

#include <sk_pixmap.h>
#include <sk_image_info_private.h>

static inline bool SkPixmapIsValid(const SkPixmap& src) {
    if (!SkImageInfoIsValid(src.info())) {
        return false;
    }

    if (!src.addr() || src.rowBytes() < src.info().minRowBytes()) {
        return false;
    }

    return true;
}