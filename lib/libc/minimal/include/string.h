#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

extern size_t strlen(const char *s);
extern size_t strnlen(const char *s, size_t maxlen);

#ifdef __cplusplus
}
#endif