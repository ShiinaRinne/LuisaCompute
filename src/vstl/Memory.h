#pragma once

#include <cstdint>
#include <cstdlib>
#include <type_traits>

#include <vstl/config.h>
#include <vstl/MetaLib.h>

void *vstl_default_malloc(size_t sz);
void vstl_default_free(void *ptr);

void *vstl_malloc(size_t size);
void vstl_free(void *ptr);

namespace vstd {

template<typename T, typename... Args>
inline T *vstl_new(Args &&...args) noexcept {
    static_assert(alignof(T) <= 16u);
    auto p = static_cast<T *>(vstl_malloc(sizeof(T)));
    return std::construct_at(p, std::forward<Args>(args)...);
}

template<typename T>
inline void vstl_delete(T *ptr) noexcept {
    if (ptr != nullptr) {
        std::destroy_at(ptr);
        vstl_free(ptr);
    }
}

#define VSTL_OVERRIDE_OPERATOR_NEW                                          \
    [[nodiscard]] static void *operator new(size_t size) noexcept {         \
        return vstl_malloc(size);                                           \
    }                                                                       \
    static void operator delete(void *p) noexcept {                         \
        vstl_free(p);                                                       \
    }                                                                       \
    [[nodiscard]] static void *operator new(size_t, void *place) noexcept { \
        return place;                                                       \
    }                                                                       \
    static void operator delete(void *pdead, size_t) noexcept {             \
        vstl_free(pdead);                                                   \
    }                                                                       \
    [[nodiscard]] static void *operator new[](size_t size) noexcept {       \
        return vstl_malloc(size);                                           \
    }                                                                       \
    static void operator delete[](void *p) noexcept {                       \
        vstl_free(p);                                                       \
    }                                                                       \
    static void operator delete[](void *pdead, size_t) noexcept {           \
        vstl_free(pdead);                                                   \
    }

class IOperatorNewBase {
public:
    virtual ~IOperatorNewBase() noexcept = default;
    VSTL_OVERRIDE_OPERATOR_NEW
};

#define VSTL_DELETE_COPY_CONSTRUCT(clsName)     \
    clsName(clsName const &) noexcept = delete; \
    clsName &operator=(clsName const &) noexcept = delete;

#define VSTL_DELETE_MOVE_CONSTRUCT(clsName) \
    clsName(clsName &&) noexcept = delete;  \
    clsName &operator=(clsName &&) noexcept = delete;

}// namespace vstd