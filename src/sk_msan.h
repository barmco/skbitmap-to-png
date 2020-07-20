#pragma once

#include <string>

#include <sk_types.h>

// Code that requires initialized inputs can call this to make it clear that
// the blame for use of uninitialized data belongs further up the call stack.
static inline void sk_msan_assert_initialized(const void* begin, const void* end) {
#if defined(__has_feature)
    #if __has_feature(memory_sanitizer)
        __msan_check_mem_is_initialized(begin, (const char*)end - (const char*)begin);
    #endif
#endif
}