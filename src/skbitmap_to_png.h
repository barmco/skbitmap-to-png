#include <stdlib.h>

struct TransformResult {
    void *encoded;
    size_t size;
};

extern "C" {
    TransformResult transform(int width, int height, size_t size, void *buf);
}