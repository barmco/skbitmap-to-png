#pragma once

#include <cassert>

#include <sk_types.h>
#include <sk_template_fits_in.h>

template <typename D, typename S> constexpr D SkTo(S s) {
    return assert(SkTFitsIn<D>(s)),
           static_cast<D>(s);
}

template <typename S> constexpr int8_t   SkToS8(S x)    { return SkTo<int8_t>(x);   }
template <typename S> constexpr uint8_t  SkToU8(S x)    { return SkTo<uint8_t>(x);  }
template <typename S> constexpr int16_t  SkToS16(S x)   { return SkTo<int16_t>(x);  }
template <typename S> constexpr uint16_t SkToU16(S x)   { return SkTo<uint16_t>(x); }
template <typename S> constexpr int32_t  SkToS32(S x)   { return SkTo<int32_t>(x);  }
template <typename S> constexpr uint32_t SkToU32(S x)   { return SkTo<uint32_t>(x); }
template <typename S> constexpr int      SkToInt(S x)   { return SkTo<int>(x);      }
template <typename S> constexpr unsigned SkToUInt(S x)  { return SkTo<unsigned>(x); }
template <typename S> constexpr size_t   SkToSizeT(S x) { return SkTo<size_t>(x);   }