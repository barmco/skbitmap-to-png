#include <cassert>

#include <sk_data_table.h>
#include <sk_malloc.h>

static SkDataTable* singleton = new SkDataTable();

static void malloc_freeproc(void* context) {
    sk_free(context);
}

// Makes empty table
SkDataTable::SkDataTable() {
    fCount = 0;
    fElemSize = 0;   // 0 signals that we use fDir instead of fElems
    fU.fDir = nullptr;
    fFreeProc = nullptr;
    fFreeProcContext = nullptr;
}

SkDataTable::SkDataTable(const void* array, size_t elemSize, int count,
                         FreeProc proc, void* context) {
    assert(count > 0);

    fCount = count;
    fElemSize = elemSize;   // non-zero signals we use fElems instead of fDir
    fU.fElems = (const char*)array;
    fFreeProc = proc;
    fFreeProcContext = context;
}

SkDataTable::SkDataTable(const Dir* dir, int count, FreeProc proc, void* ctx) {
    assert(count > 0);

    fCount = count;
    fElemSize = 0;  // 0 signals that we use fDir instead of fElems
    fU.fDir = dir;
    fFreeProc = proc;
    fFreeProcContext = ctx;
}

SkDataTable::~SkDataTable() {
    if (fFreeProc) {
        fFreeProc(fFreeProcContext);
    }
}

size_t SkDataTable::atSize(int index) const {
    assert((unsigned)index < (unsigned)fCount);

    if (fElemSize) {
        return fElemSize;
    } else {
        return fU.fDir[index].fSize;
    }
}

const void* SkDataTable::at(int index, size_t* size) const {
    assert((unsigned)index < (unsigned)fCount);

    if (fElemSize) {
        if (size) {
            *size = fElemSize;
        }
        return fU.fElems + index * fElemSize;
    } else {
        if (size) {
            *size = fU.fDir[index].fSize;
        }
        return fU.fDir[index].fPtr;
    }
}

///////////////////////////////////////////////////////////////////////////////

SkDataTable* SkDataTable::MakeEmpty() {
    return singleton;
}

SkDataTable* SkDataTable::MakeCopyArrays(const void * const * ptrs,
                                               const size_t sizes[], int count) {
    if (count <= 0) {
        return SkDataTable::MakeEmpty();
    }

    size_t dataSize = 0;
    for (int i = 0; i < count; ++i) {
        dataSize += sizes[i];
    }

    size_t bufferSize = count * sizeof(Dir) + dataSize;
    void* buffer = sk_malloc_throw(bufferSize);

    Dir* dir = (Dir*)buffer;
    char* elem = (char*)(dir + count);
    for (int i = 0; i < count; ++i) {
        dir[i].fPtr = elem;
        dir[i].fSize = sizes[i];
        memcpy(elem, ptrs[i], sizes[i]);
        elem += sizes[i];
    }

    return new SkDataTable(dir, count, malloc_freeproc, buffer);
}

SkDataTable* SkDataTable::MakeCopyArray(const void* array, size_t elemSize, int count) {
    if (count <= 0) {
        return SkDataTable::MakeEmpty();
    }

    size_t bufferSize = elemSize * count;
    void* buffer = sk_malloc_throw(bufferSize);
    memcpy(buffer, array, bufferSize);

    return new SkDataTable(buffer, elemSize, count, malloc_freeproc, buffer);
}

SkDataTable* SkDataTable::MakeArrayProc(const void* array, size_t elemSize, int count,
                                              FreeProc proc, void* ctx) {
    if (count <= 0) {
        return SkDataTable::MakeEmpty();
    }
    return new SkDataTable(array, elemSize, count, proc, ctx);
}