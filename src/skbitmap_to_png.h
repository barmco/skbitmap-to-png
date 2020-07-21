#include <stdlib.h>

struct TransformResult {
    void *handle;
    void *encoded;
    size_t size;
};

extern "C" {
    TransformResult transform_to_png(int width, int height, size_t size, void *buf);
    TransformResult transform_to_bgra8888(int width, int height, size_t size, void *buf);
    
    void memfree(void *handle);
}