#include <vector_wstream.h>

bool VectorWStream::write(const void* buffer, size_t size) {
  const unsigned char* ptr = reinterpret_cast<const unsigned char*>(buffer);
  dst_->insert(dst_->end(), ptr, ptr + size);
  return true;
}

size_t VectorWStream::bytesWritten() const {
  return dst_->size();
}