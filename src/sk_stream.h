#pragma once

#include <stddef.h>

#include <memory>
#include <cassert>

#include <sk_types.h>
#include <sk_scalar.h>
#include <sk_to_private.h>

/**
 *  SkStream -- abstraction for a source of bytes. Subclasses can be backed by
 *  memory, or a file, or something else.
 *
 *  NOTE:
 *
 *  Classic "streams" APIs are sort of async, in that on a request for N
 *  bytes, they may return fewer than N bytes on a given call, in which case
 *  the caller can "try again" to get more bytes, eventually (modulo an error)
 *  receiving their total N bytes.
 *
 *  Skia streams behave differently. They are effectively synchronous, and will
 *  always return all N bytes of the request if possible. If they return fewer
 *  (the read() call returns the number of bytes read) then that means there is
 *  no more data (at EOF or hit an error). The caller should *not* call again
 *  in hopes of fulfilling more of the request.
 */
class SkStream {
public:
    virtual ~SkStream() {}
    SkStream() {}

    /** Reads or skips size number of bytes.
     *  If buffer == NULL, skip size bytes, return how many were skipped.
     *  If buffer != NULL, copy size bytes into buffer, return how many were copied.
     *  @param buffer when NULL skip size bytes, otherwise copy size bytes into buffer
     *  @param size the number of bytes to skip or copy
     *  @return the number of bytes actually read.
     */
    virtual size_t read(void* buffer, size_t size) = 0;

    /** Skip size number of bytes.
     *  @return the actual number bytes that could be skipped.
     */
    size_t skip(size_t size) {
        return this->read(nullptr, size);
    }

    /**
     *  Attempt to peek at size bytes.
     *  If this stream supports peeking, copy min(size, peekable bytes) into
     *  buffer, and return the number of bytes copied.
     *  If the stream does not support peeking, or cannot peek any bytes,
     *  return 0 and leave buffer unchanged.
     *  The stream is guaranteed to be in the same visible state after this
     *  call, regardless of success or failure.
     *  @param buffer Must not be NULL, and must be at least size bytes. Destination
     *      to copy bytes.
     *  @param size Number of bytes to copy.
     *  @return The number of bytes peeked/copied.
     */
    virtual size_t peek(void* /*buffer*/, size_t /*size*/) const { return 0; }

    /** Returns true when all the bytes in the stream have been read.
     *  This may return true early (when there are no more bytes to be read)
     *  or late (after the first unsuccessful read).
     */
    virtual bool isAtEnd() const = 0;

    bool readS8(int8_t*);
    bool readS16(int16_t*);
    bool readS32(int32_t*);

    bool readU8(uint8_t* i) { return this->readS8((int8_t*)i); }
    bool readU16(uint16_t* i) { return this->readS16((int16_t*)i); }
    bool readU32(uint32_t* i) { return this->readS32((int32_t*)i); }

    bool readBool(bool* b) {
        uint8_t i;
        if (!this->readU8(&i)) { return false; }
        *b = (i != 0);
        return true;
    }
    bool readScalar(SkScalar*);
    bool readPackedUInt(size_t*);

//SkStreamRewindable
    /** Rewinds to the beginning of the stream. Returns true if the stream is known
     *  to be at the beginning after this call returns.
     */
    virtual bool rewind() { return false; }

    /** Duplicates this stream. If this cannot be done, returns NULL.
     *  The returned stream will be positioned at the beginning of its data.
     */
    std::unique_ptr<SkStream> duplicate() const {
        return std::unique_ptr<SkStream>(this->onDuplicate());
    }
    /** Duplicates this stream. If this cannot be done, returns NULL.
     *  The returned stream will be positioned the same as this stream.
     */
    std::unique_ptr<SkStream> fork() const {
        return std::unique_ptr<SkStream>(this->onFork());
    }

//SkStreamSeekable
    /** Returns true if this stream can report it's current position. */
    virtual bool hasPosition() const { return false; }
    /** Returns the current position in the stream. If this cannot be done, returns 0. */
    virtual size_t getPosition() const { return 0; }

    /** Seeks to an absolute position in the stream. If this cannot be done, returns false.
     *  If an attempt is made to seek past the end of the stream, the position will be set
     *  to the end of the stream.
     */
    virtual bool seek(size_t /*position*/) { return false; }

    /** Seeks to an relative offset in the stream. If this cannot be done, returns false.
     *  If an attempt is made to move to a position outside the stream, the position will be set
     *  to the closest point within the stream (beginning or end).
     */
    virtual bool move(long /*offset*/) { return false; }

//SkStreamAsset
    /** Returns true if this stream can report it's total length. */
    virtual bool hasLength() const { return false; }
    /** Returns the total length of the stream. If this cannot be done, returns 0. */
    virtual size_t getLength() const { return 0; }

//SkStreamMemory
    /** Returns the starting address for the data. If this cannot be done, returns NULL. */
    //TODO: replace with virtual const SkData* getData()
    virtual const void* getMemoryBase() { return nullptr; }

private:
    virtual SkStream* onDuplicate() const { return nullptr; }
    virtual SkStream* onFork() const { return nullptr; }

    SkStream(SkStream&&) = delete;
    SkStream(const SkStream&) = delete;
    SkStream& operator=(SkStream&&) = delete;
    SkStream& operator=(const SkStream&) = delete;
};

/** SkStreamRewindable is a SkStream for which rewind and duplicate are required. */
class SkStreamRewindable : public SkStream {
public:
    bool rewind() override = 0;
    std::unique_ptr<SkStreamRewindable> duplicate() const {
        return std::unique_ptr<SkStreamRewindable>(this->onDuplicate());
    }
private:
    SkStreamRewindable* onDuplicate() const override = 0;
};

class SkWStream {
public:
    virtual ~SkWStream();
    SkWStream() {}

    /** Called to write bytes to a SkWStream. Returns true on success
        @param buffer the address of at least size bytes to be written to the stream
        @param size The number of bytes in buffer to write to the stream
        @return true on success
    */
    virtual bool write(const void* buffer, size_t size) = 0;
    virtual void flush();

    virtual size_t bytesWritten() const = 0;

    // helpers

    bool write8(U8CPU value)   {
        uint8_t v = SkToU8(value);
        return this->write(&v, 1);
    }
    bool write16(U16CPU value) {
        uint16_t v = SkToU16(value);
        return this->write(&v, 2);
    }
    bool write32(uint32_t v) {
        return this->write(&v, 4);
    }

    bool writeText(const char text[]) {
        assert(text);
        return this->write(text, strlen(text));
    }

    bool newline() { return this->write("\n", strlen("\n")); }

    bool writeDecAsText(int32_t);
    bool writeBigDecAsText(int64_t, int minDigits = 0);

    bool writeBool(bool v) { return this->write8(v); }
    bool writeScalar(SkScalar);
    bool writePackedUInt(size_t);

    bool writeStream(SkStream* input, size_t length);

    /**
     * This returns the number of bytes in the stream required to store
     * 'value'.
     */
    static int SizeOfPackedUInt(size_t value);

private:
    SkWStream(const SkWStream&) = delete;
    SkWStream& operator=(const SkWStream&) = delete;
};