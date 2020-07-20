#include <stdlib.h>

#define SkStrAppendU32_MaxSize  10
char*   SkStrAppendU32(char buffer[], uint32_t);
#define SkStrAppendU64_MaxSize  20
char*   SkStrAppendU64(char buffer[], uint64_t, int minDigits);

#define SkStrAppendS32_MaxSize  (SkStrAppendU32_MaxSize + 1)
char*   SkStrAppendS32(char buffer[], int32_t);
#define SkStrAppendS64_MaxSize  (SkStrAppendU64_MaxSize + 1)
char*   SkStrAppendS64(char buffer[], int64_t, int minDigits);