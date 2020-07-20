#pragma once

#include <memory>
#include <type_traits>

#include <sk_malloc.h>

namespace sknonstd {
    // The name 'copy' here is fraught with peril. In this case it means 'append', not 'overwrite'.
    // Alternate proposed names are 'propagate', 'augment', or 'append' (and 'add', but already taken).
    // std::experimental::propagate_const already exists for other purposes in TSv2.
    // These also follow the <dest, source> pattern used by boost.
    template <typename D, typename S> struct copy_const {
        using type = std::conditional_t<std::is_const<S>::value, std::add_const_t<D>, D>;
    };
    template <typename D, typename S> using copy_const_t = typename copy_const<D, S>::type;

    template <typename D, typename S> struct copy_volatile {
        using type = std::conditional_t<std::is_volatile<S>::value, std::add_volatile_t<D>, D>;
    };
    template <typename D, typename S> using copy_volatile_t = typename copy_volatile<D, S>::type;

    template <typename D, typename S> struct copy_cv {
        using type = copy_volatile_t<copy_const_t<D, S>, S>;
    };
    template <typename D, typename S> using copy_cv_t = typename copy_cv<D, S>::type;

    // The name 'same' here means 'overwrite'.
    // Alternate proposed names are 'replace', 'transfer', or 'qualify_from'.
    // same_xxx<D, S> can be written as copy_xxx<remove_xxx_t<D>, S>
    template <typename D, typename S> using same_const = copy_const<std::remove_const_t<D>, S>;
    template <typename D, typename S> using same_const_t = typename same_const<D, S>::type;
    template <typename D, typename S> using same_volatile =copy_volatile<std::remove_volatile_t<D>,S>;
    template <typename D, typename S> using same_volatile_t = typename same_volatile<D, S>::type;
    template <typename D, typename S> using same_cv = copy_cv<std::remove_cv_t<D>, S>;
    template <typename D, typename S> using same_cv_t = typename same_cv<D, S>::type;
}

// TODO: when C++17 the language is available, use template <auto P>
template <typename T, T* P> struct SkFunctionWrapper {
    template <typename... Args>
    auto operator()(Args&&... args) const -> decltype(P(std::forward<Args>(args)...)) {
        return P(std::forward<Args>(args)...);
    }
};

/** Manages an array of T elements, freeing the array in the destructor.
 *  Does NOT call any constructors/destructors on T (T must be POD).
 */
template <typename T> class SkAutoTMalloc  {
public:
    /** Takes ownership of the ptr. The ptr must be a value which can be passed to sk_free. */
    explicit SkAutoTMalloc(T* ptr = nullptr) : fPtr(ptr) {}

    /** Allocates space for 'count' Ts. */
    explicit SkAutoTMalloc(size_t count)
        : fPtr(count ? (T*)sk_malloc_throw(count, sizeof(T)) : nullptr) {}

    SkAutoTMalloc(SkAutoTMalloc&&) = default;
    SkAutoTMalloc& operator=(SkAutoTMalloc&&) = default;

    /** Resize the memory area pointed to by the current ptr preserving contents. */
    void realloc(size_t count) {
        fPtr.reset(count ? (T*)sk_realloc_throw(fPtr.release(), count * sizeof(T)) : nullptr);
    }

    /** Resize the memory area pointed to by the current ptr without preserving contents. */
    T* reset(size_t count = 0) {
        fPtr.reset(count ? (T*)sk_malloc_throw(count, sizeof(T)) : nullptr);
        return this->get();
    }

    T* get() const { return fPtr.get(); }

    operator T*() { return fPtr.get(); }

    operator const T*() const { return fPtr.get(); }

    T& operator[](int index) { return fPtr.get()[index]; }

    const T& operator[](int index) const { return fPtr.get()[index]; }

    /**
     *  Transfer ownership of the ptr to the caller, setting the internal
     *  pointer to NULL. Note that this differs from get(), which also returns
     *  the pointer, but it does not transfer ownership.
     */
    T* release() { return fPtr.release(); }

private:
    std::unique_ptr<T, SkFunctionWrapper<void(void*), sk_free>> fPtr;
};

/**
 *  Returns a pointer to a D which comes byteOffset bytes after S.
 */
template <typename D, typename S> static D* SkTAddOffset(S* ptr, size_t byteOffset) {
    // The intermediate char* has the same cv-ness as D as this produces better error messages.
    // This relies on the fact that reinterpret_cast can add constness, but cannot remove it.
    return reinterpret_cast<D*>(reinterpret_cast<sknonstd::same_cv_t<char, D>*>(ptr) + byteOffset);
}