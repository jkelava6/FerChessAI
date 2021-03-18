#pragma once





#define ARRAY_SIZE(Array) (sizeof(Array) / sizeof(*Array))
#define ARRAY_MATCH(RefArray, OtherArray) static_assert(ARRAY_SIZE(RefArray) == ARRAY_SIZE(OtherArray), "Array size mismatch!")

#define _CRT_SECURE_NO_WARNINGS


