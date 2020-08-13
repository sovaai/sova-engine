#pragma once

template <typename T, size_t N> char (&ArraySizeHelper(T (&array)[N]))[N];
#define ARRAY_SIZE(array) (sizeof(ArraySizeHelper(array)))

#define STR_SIZE(str) (sizeof(ArraySizeHelper(str)) - 1)
