#pragma once

#include <cassert>
#include <vector>

#include <sk_stream.h>

class VectorWStream : public SkWStream {
 public:
  // We do not take ownership of dst
  VectorWStream(std::vector<unsigned char>* dst) : dst_(dst) {
    assert(dst_);
    assert(dst->size() == 0UL);
  }

  bool write(const void* buffer, size_t size) override;

  size_t bytesWritten() const override;

 private:
  // Does not have ownership.
  std::vector<unsigned char>* dst_;
};