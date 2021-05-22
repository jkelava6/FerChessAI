#pragma once



typedef unsigned int uint;

#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof(*Array))
#define ARRAY_MATCH(RefArray, OtherArray) static_assert(ARRAY_SIZE(RefArray) == ARRAY_SIZE(OtherArray), "Array size mismatch!")

#define DECLARE_NOCOPY(ClassName) \
ClassName& operator= (const ClassName&) = delete; \
ClassName(const ClassName&) = delete
#define DECLARE_NOMOVE(ClassName) \
ClassName& operator= (ClassName&&) = delete; \
ClassName(ClassName&&) = delete

#define DECLARE_COPY(ClassName) \
ClassName& operator= (const ClassName&); \
ClassName(const ClassName&)
#define DECLARE_MOVE(ClassName) \
ClassName& operator= (ClassName&&); \
ClassName(ClassName&&)

#define IMPLEMENT_COPY(ClassName) \
ClassName& ClassName::operator= (const ClassName&) = default; \
ClassName::ClassName(const ClassName&) = default
#define IMPLEMENT_MOVE(ClassName) \
ClassName& ClassName::operator= (ClassName&&) = default; \
ClassName::ClassName(ClassName&&) = default

template <class Type>
Type&& Move(Type& Moved) noexcept
{
    return static_cast<Type&&>(Moved);
}

#define RANKS 6
#define FILES 6
#define DEFAULT_BOARD (RANKS == 8 && FILES == 8)


