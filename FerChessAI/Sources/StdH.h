#pragma once





#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof(*Array))
#define ARRAY_MATCH(RefArray, OtherArray) static_assert(ARRAY_SIZE(RefArray) == ARRAY_SIZE(OtherArray), "Array size mismatch!")

#define DECLARE_NOCOPY(ClassName) \
ClassName operator= (const ClassName&) = delete; \
ClassName(const ClassName&) = delete
#define DECLARE_NOMOVE(ClassName) \
ClassName operator= (ClassName&&) = delete; \
ClassName(ClassName&&) = delete

#define _CRT_SECURE_NO_WARNINGS


