#include <stdlib.h>

struct TransformResult {
    void *handle;
    void *encoded;
    size_t size;
};

extern "C" {
    TransformResult transform_to_png(int width, int height, size_t size, void *buf);
    TransformResult transform_to_bgra8888(int width, int height, size_t size, void *buf);
    size_t compute_min_bytesize(int width, int height);

    void memfree(void *handle);
}