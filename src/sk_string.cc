#include <cassert>
#include <memory>

#include <sk_string.h>
#include <sk_to_private.h>

char* SkStrAppendU32(char string[], uint32_t dec) {
    char* start = string;

    char    buffer[SkStrAppendU32_MaxSize];
    char*   p = buffer + sizeof(buffer);

    do {
        *--p = SkToU8('0' + dec % 10);
        dec /= 10;
    } while (dec != 0);

    assert(p >= buffer);
    char* stop = buffer + sizeof(buffer);
    while (p < stop) {
        *string++ = *p++;
    }
    assert(string - start <= SkStrAppendU32_MaxSize);
    return string;
}

char* SkStrAppendS32(char string[], int32_t dec) {
    uint32_t udec = dec;
    if (dec < 0) {
        *string++ = '-';
        udec = ~udec + 1;  // udec = -udec, but silences some warnings that are trying to be helpful
    }
    return SkStrAppendU32(string, udec);
}

char* SkStrAppendU64(char string[], uint64_t dec, int minDigits) {
    char* start = string;

    char    buffer[SkStrAppendU64_MaxSize];
    char*   p = buffer + sizeof(buffer);

    do {
        *--p = SkToU8('0' + (int32_t) (dec % 10));
        dec /= 10;
        minDigits--;
    } while (dec != 0);

    while (minDigits > 0) {
        *--p = '0';
        minDigits--;
    }

    assert(p >= buffer);
    size_t cp_len = buffer + sizeof(buffer) - p;
    memcpy(string, p, cp_len);
    string += cp_len;

    assert(string - start <= SkStrAppendU64_MaxSize);
    return string;
}

char* SkStrAppendS64(char string[], int64_t dec, int minDigits) {
    uint64_t udec = dec;
    if (dec < 0) {
        *string++ = '-';
        udec = ~udec + 1;  // udec = -udec, but silences some warnings that are trying to be helpful
    }
    return SkStrAppendU64(string, udec, minDigits);
}
