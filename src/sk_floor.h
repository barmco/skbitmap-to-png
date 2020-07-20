#pragma once

#include <math.h>

#include <sk_types.h>

constexpr float SK_FloatSqrt2 = 1.41421356f;
constexpr float SK_FloatPI    = 3.14159265f;
constexpr double SK_DoublePI  = 3.14159265358979323846264338327950288;

// C++98 cmath std::pow seems to be the earliest portable way to get float pow.
// However, on Linux including cmath undefines isfinite.
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=14608
static inline float sk_float_pow(float base, float exp) {
    return powf(base, exp);
}

#define sk_float_sqrt(x)        sqrtf(x)
#define sk_float_sin(x)         sinf(x)
#define sk_float_cos(x)         cosf(x)
#define sk_float_tan(x)         tanf(x)
#define sk_float_floor(x)       floorf(x)
#define sk_float_ceil(x)        ceilf(x)
#define sk_float_trunc(x)       truncf(x)
#ifdef SK_BUILD_FOR_MAC
#    define sk_float_acos(x)    static_cast<float>(acos(x))
#    define sk_float_asin(x)    static_cast<float>(asin(x))
#else
#    define sk_float_acos(x)    acosf(x)
#    define sk_float_asin(x)    asinf(x)
#endif
#define sk_float_atan2(y,x)     atan2f(y,x)
#define sk_float_abs(x)         fabsf(x)
#define sk_float_copysign(x, y) copysignf(x, y)
#define sk_float_mod(x,y)       fmodf(x,y)
#define sk_float_exp(x)         expf(x)
#define sk_float_log(x)         logf(x)


#define SK_MaxS32FitsInFloat    2147483520
#define SK_MinS32FitsInFloat    -SK_MaxS32FitsInFloat

#define SK_MaxS64FitsInFloat    (SK_MaxS64 >> (63-24) << (63-24))   // 0x7fffff8000000000
#define SK_MinS64FitsInFloat    -SK_MaxS64FitsInFloat

/**
 *  Return the closest int for the given float. Returns SK_MaxS32FitsInFloat for NaN.
 */
static inline int sk_float_saturate2int(float x) {
    x = x < SK_MaxS32FitsInFloat ? x : SK_MaxS32FitsInFloat;
    x = x > SK_MinS32FitsInFloat ? x : SK_MinS32FitsInFloat;
    return (int)x;
}

/**
 *  Return the closest int for the given double. Returns SK_MaxS32 for NaN.
 */
static inline int sk_double_saturate2int(double x) {
    x = x < SK_MaxS32 ? x : SK_MaxS32;
    x = x > SK_MinS32 ? x : SK_MinS32;
    return (int)x;
}

/**
 *  Return the closest int64_t for the given float. Returns SK_MaxS64FitsInFloat for NaN.
 */
static inline int64_t sk_float_saturate2int64(float x) {
    x = x < SK_MaxS64FitsInFloat ? x : SK_MaxS64FitsInFloat;
    x = x > SK_MinS64FitsInFloat ? x : SK_MinS64FitsInFloat;
    return (int64_t)x;
}

#define sk_float_floor2int(x)   sk_float_saturate2int(sk_float_floor(x))
#define sk_float_round2int(x)   sk_float_saturate2int(sk_float_floor((x) + 0.5f))
#define sk_float_ceil2int(x)    sk_float_saturate2int(sk_float_ceil(x))

#define sk_float_floor2int_no_saturate(x)   (int)sk_float_floor(x)
#define sk_float_round2int_no_saturate(x)   (int)sk_float_floor((x) + 0.5f)
#define sk_float_ceil2int_no_saturate(x)    (int)sk_float_ceil(x)

#define sk_double_floor(x)          floor(x)
#define sk_double_round(x)          floor((x) + 0.5)
#define sk_double_ceil(x)           ceil(x)
#define sk_double_floor2int(x)      (int)floor(x)
#define sk_double_round2int(x)      (int)floor((x) + 0.5)
#define sk_double_ceil2int(x)       (int)ceil(x)