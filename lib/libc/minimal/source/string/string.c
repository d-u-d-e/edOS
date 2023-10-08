#include <stdint.h>
#include <stddef.h>

size_t strnlen(const char *s, size_t maxlen) {
  size_t n = 0;

  while (*s != '\0' && n < maxlen) {
    s++;
    n++;
  }

  return n;
}

size_t strlen(const char *s)
{
	size_t n = 0;

	while (*s != '\0') {
		s++;
		n++;
	}

	return n;
}
