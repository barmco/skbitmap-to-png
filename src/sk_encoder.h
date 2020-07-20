#pragma once

#include <sk_pixmap.h>
#include <sk_templates_private.h>

class SkEncoder {
public:

    /**
     *  Encode |numRows| rows of input.  If the caller requests more rows than are remaining
     *  in the src, this will encode all of the remaining rows.  |numRows| must be greater
     *  than zero.
     */
    bool encodeRows(int numRows);

    virtual ~SkEncoder() {}

protected:

    virtual bool onEncodeRows(int numRows) = 0;

    SkEncoder(const SkPixmap& src, size_t storageBytes)
        : fSrc(src)
        , fCurrRow(0)
        , fStorage(storageBytes)
    {}

    const SkPixmap&        fSrc;
    int                    fCurrRow;
    SkAutoTMalloc<uint8_t> fStorage;
};
