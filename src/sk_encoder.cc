#include <cassert>

#include <sk_encoder.h>

bool SkEncoder::encodeRows(int numRows) {
    assert(numRows > 0 && fCurrRow < fSrc.height());
    if (numRows <= 0 || fCurrRow >= fSrc.height()) {
        return false;
    }

    if (fCurrRow + numRows > fSrc.height()) {
        numRows = fSrc.height() - fCurrRow;
    }

    if (!this->onEncodeRows(numRows)) {
        // If we fail, short circuit any future calls.
        fCurrRow = fSrc.height();
        return false;
    }

    return true;
}