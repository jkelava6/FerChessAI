#pragma once



typedef unsigned int uint;

#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof(*Array))
#define ARRAY_MATCH(RefArray, OtherArray) static_assert(ARRAY_SIZE(RefArray) == ARRAY_SIZE(OtherArray), "Array size mismatch!")

#define DECLARE_NOCOPY(ClassName) \
ClassName operator= (const ClassName&) = delete; \
ClassName(const ClassName&) = delete
#define DECLARE_NOMOVE(ClassName) \
ClassName operator= (ClassName&&) = delete; \
ClassName(ClassName&&) = delete

template <class Type>
Type&& Move(Type& LValue)
{
    return static_cast<Type&&>(LValue);
}



